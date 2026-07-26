// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/logs/event_to_span_event_bridge_processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
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
      : attributes_(attributes)
  {}

  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback)
      const noexcept override
  {
    bool keep_going = true;
    for (const auto &kv : attributes_)
    {
      if (!keep_going)
      {
        break;
      }
      EventAttributeEmitter emitter(kv.first, callback, &keep_going);
      opentelemetry::nostd::visit(emitter, kv.second);
    }
    return keep_going;
  }

  std::size_t size() const noexcept override { return attributes_.size(); }

private:
  const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
      &attributes_;
};

}  // namespace

std::unique_ptr<Recordable> EventToSpanEventBridgeProcessor::MakeRecordable() noexcept
{
  return std::unique_ptr<Recordable>(new ReadWriteLogRecord());
}

void EventToSpanEventBridgeProcessor::OnEmit(std::unique_ptr<Recordable> &&record) noexcept
{
  std::unique_ptr<ReadWriteLogRecord> log_record(
      static_cast<ReadWriteLogRecord *>(record.release()));
  if (!log_record)
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

  auto current_span = opentelemetry::trace::Tracer::GetCurrentSpan();
  if (!current_span || !current_span->IsRecording())
  {
    return;
  }

  opentelemetry::trace::SpanContext current_context = current_span->GetContext();
  if (!(current_context.trace_id() == log_trace_id) || !(current_context.span_id() == log_span_id))
  {
    return;
  }

  opentelemetry::common::SystemTimestamp event_timestamp = log_record->GetTimestamp();
  if (event_timestamp.time_since_epoch().count() == 0)
  {
    event_timestamp = log_record->GetObservedTimestamp();
  }

  EventAttributesKeyValueIterable attributes(log_record->GetAttributes());
  current_span->AddEvent(event_name, event_timestamp, attributes);
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
