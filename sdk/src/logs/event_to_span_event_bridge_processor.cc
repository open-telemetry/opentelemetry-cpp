// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

namespace
{

// Converts a single OwnedAttributeValue into the corresponding AttributeValue and forwards it
// to the KeyValueIterable callback. Array-of-string and array-of-bool attributes need a
// temporary contiguous buffer (std::vector<std::string> does not decay to a span of
// string_view, and std::vector<bool> is not backed by a real bool array), so those buffers are
// kept alive for the duration of a single callback invocation only.
class EventAttributeEmitter
{
public:
  EventAttributeEmitter(
      nostd::string_view key,
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback,
      bool *keep_going)
      : key_(key), callback_(callback), keep_going_(keep_going)
  {}

  void operator()(bool v) { Emit(v); }
  void operator()(int32_t v) { Emit(v); }
  void operator()(uint32_t v) { Emit(v); }
  void operator()(int64_t v) { Emit(v); }
  void operator()(uint64_t v) { Emit(v); }
  void operator()(double v) { Emit(v); }
  void operator()(const std::string &v) { Emit(nostd::string_view(v)); }

  void operator()(const std::vector<bool> &v)
  {
    std::unique_ptr<bool[]> buffer(new bool[v.size()]);
    for (std::size_t i = 0; i < v.size(); ++i)
    {
      buffer[i] = v[i];
    }
    Emit(nostd::span<const bool>(buffer.get(), v.size()));
  }

  void operator()(const std::vector<int32_t> &v)
  {
    Emit(nostd::span<const int32_t>(v.data(), v.size()));
  }

  void operator()(const std::vector<uint32_t> &v)
  {
    Emit(nostd::span<const uint32_t>(v.data(), v.size()));
  }

  void operator()(const std::vector<int64_t> &v)
  {
    Emit(nostd::span<const int64_t>(v.data(), v.size()));
  }

  void operator()(const std::vector<uint64_t> &v)
  {
    Emit(nostd::span<const uint64_t>(v.data(), v.size()));
  }

  void operator()(const std::vector<double> &v)
  {
    Emit(nostd::span<const double>(v.data(), v.size()));
  }

  void operator()(const std::vector<uint8_t> &v)
  {
    Emit(nostd::span<const uint8_t>(v.data(), v.size()));
  }

  void operator()(const std::vector<std::string> &v)
  {
    std::vector<nostd::string_view> views(v.begin(), v.end());
    Emit(nostd::span<const nostd::string_view>(views.data(), views.size()));
  }

private:
  template <class T>
  void Emit(const T &value)
  {
    if (*keep_going_)
    {
      *keep_going_ = callback_(key_, opentelemetry::common::AttributeValue(value));
    }
  }

  nostd::string_view key_;
  nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback_;
  bool *keep_going_;
};

class EventAttributesKeyValueIterable final : public opentelemetry::common::KeyValueIterable
{
public:
  explicit EventAttributesKeyValueIterable(
      const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
          &attributes)
      : attributes_(&attributes)
  {}

  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback)
      const noexcept override
  {
    // Building temporary buffers for array-of-bool and array-of-string attributes (see
    // EventAttributeEmitter above) can allocate, so guard against a thrown bad_alloc escaping
    // this noexcept override; conservatively stop iterating rather than terminate the process.
#if OPENTELEMETRY_HAVE_EXCEPTIONS
    try
    {
#endif
      bool keep_going = true;
      for (const auto &kv : *attributes_)
      {
        if (!keep_going)
        {
          break;
        }
        EventAttributeEmitter emitter(kv.first, callback, &keep_going);
        opentelemetry::nostd::visit(emitter, kv.second);
      }
      return keep_going;
#if OPENTELEMETRY_HAVE_EXCEPTIONS
    }
    catch (...)
    {
      return false;
    }
#endif
  }

  std::size_t size() const noexcept override { return attributes_->size(); }

private:
  const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
      *attributes_;
};

// Bridges `record` onto `span` as a span event, provided every precondition holds:
// the record is an event (non-empty event name), it carries a valid trace id and span id,
// `span` is a live recording span, and `span`'s ids match the ones on the record.
// Any failed precondition means the record is not an event for this span, and it is left
// to continue through the rest of the pipeline untouched.
void BridgeRecordToSpan(const ReadWriteLogRecord *log_record,
                        const nostd::shared_ptr<opentelemetry::trace::Span> &span) noexcept
{
  if (log_record == nullptr)
  {
    return;
  }

  nostd::string_view event_name = log_record->GetEventName();
  if (event_name.empty())
  {
    return;
  }

  const opentelemetry::trace::TraceId &log_trace_id = log_record->GetTraceId();
  const opentelemetry::trace::SpanId &log_span_id   = log_record->GetSpanId();
  if (!log_trace_id.IsValid() || !log_span_id.IsValid())
  {
    return;
  }

  // trace::GetSpan() yields a non-recording DefaultSpan when the context carries no span,
  // so the IsRecording() check also covers the "no span in context" case.
  if (!span || !span->IsRecording())
  {
    return;
  }

  const opentelemetry::trace::SpanContext span_context = span->GetContext();
  if (!(span_context.trace_id() == log_trace_id) || !(span_context.span_id() == log_span_id))
  {
    return;
  }

  opentelemetry::common::SystemTimestamp event_timestamp = log_record->GetTimestamp();
  if (event_timestamp.time_since_epoch().count() == 0)
  {
    event_timestamp = log_record->GetObservedTimestamp();
  }

  EventAttributesKeyValueIterable attributes(log_record->GetAttributes());
  span->AddEvent(event_name, event_timestamp, attributes);
}

}  // namespace

std::unique_ptr<Recordable> EventToSpanEventBridgeProcessor::MakeRecordable() noexcept
{
  return std::unique_ptr<Recordable>(new ReadWriteLogRecord());
}

void EventToSpanEventBridgeProcessor::OnEmit(std::unique_ptr<Recordable> &&record) noexcept
{
  const std::unique_ptr<ReadWriteLogRecord> log_record(
      static_cast<ReadWriteLogRecord *>(std::move(record).release()));

  // No resolved context supplied, so the ambient span is the only span reachable here.
  BridgeRecordToSpan(log_record.get(), opentelemetry::trace::Tracer::GetCurrentSpan());
}

void EventToSpanEventBridgeProcessor::OnEmitWithContext(
    std::unique_ptr<Recordable> &&record,
    const opentelemetry::nostd::variant<opentelemetry::trace::SpanContext,
                                        opentelemetry::context::Context> &context) noexcept
{
  const std::unique_ptr<ReadWriteLogRecord> log_record(
      static_cast<ReadWriteLogRecord *>(std::move(record).release()));

  if (nostd::holds_alternative<opentelemetry::context::Context>(context))
  {
    // The resolved context carries the live Span the record belongs to. Add the event to that
    // span directly, which is what makes an explicitly supplied context work even when a
    // different span is ambient.
    BridgeRecordToSpan(log_record.get(), opentelemetry::trace::GetSpan(
                                             nostd::get<opentelemetry::context::Context>(context)));
    return;
  }

  // A bare SpanContext carries only ids, not a live Span, and the SDK offers no way to look up
  // a Span by id. The ambient span is therefore the only span that can be written to; the id
  // check in BridgeRecordToSpan drops the record when it is not the one the ids refer to.
  BridgeRecordToSpan(log_record.get(), opentelemetry::trace::Tracer::GetCurrentSpan());
}

bool EventToSpanEventBridgeProcessor::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  return true;
}

bool EventToSpanEventBridgeProcessor::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  return true;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
