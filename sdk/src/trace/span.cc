#include "opentelemetry/sdk/trace/span.h"

#include "src/common/random.h"

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

using opentelemetry::core::SteadyTimestamp;
using opentelemetry::core::SystemTimestamp;

namespace
{
SystemTimestamp NowOr(const SystemTimestamp &system)
{
  if (system == SystemTimestamp())
  {
    return SystemTimestamp(std::chrono::system_clock::now());
  }
  else
  {
    return system;
  }
}

SteadyTimestamp NowOr(const SteadyTimestamp &steady)
{
  if (steady == SteadyTimestamp())
  {
    return SteadyTimestamp(std::chrono::steady_clock::now());
  }
  else
  {
    return steady;
  }
}
}  // namespace

// Helper function to generate random trace id.
trace_api::TraceId GenerateRandomTraceId()
{
  uint8_t trace_id_buf[trace_api::TraceId::kSize];
  sdk::common::Random::GenerateRandomBuffer(trace_id_buf);
  return trace_api::TraceId(trace_id_buf);
}

// Helper function to generate random span id.
trace_api::SpanId GenerateRandomSpanId()
{
  uint8_t span_id_buf[trace_api::SpanId::kSize];
  sdk::common::Random::GenerateRandomBuffer(span_id_buf);
  return trace_api::SpanId(span_id_buf);
}

Span::Span(std::shared_ptr<Tracer> &&tracer,
           nostd::string_view name,
           const opentelemetry::common::KeyValueIterable &attributes,
           const trace_api::SpanContextKeyValueIterable &links,
           const trace_api::StartSpanOptions &options,
           const trace_api::SpanContext &parent_span_context) noexcept
    : exportable_(new ExportableSpan(std::move(tracer))),
      start_steady_time{options.start_steady_time},
      has_ended_{false}
{
  // Register exportables.   TODO - sampling?
  exportable_->GetTracer().GetActiveProcessor().RegisterRecordable(*exportable_);
  auto* recordable = GetRecordable();
  if (recordable== nullptr)
  {
    return;
  }

  recordable->SetName(name);

  trace_api::TraceId trace_id;
  trace_api::SpanId span_id = GenerateRandomSpanId();
  bool is_parent_span_valid = false;

  if (parent_span_context.IsValid())
  {
    trace_id = parent_span_context.trace_id();
    recordable->SetIds(trace_id, span_id, parent_span_context.span_id());
    is_parent_span_valid = true;
  }
  else
  {
    trace_id = GenerateRandomTraceId();
    recordable->SetIds(trace_id, span_id, trace_api::SpanId());
  }

  span_context_ = std::unique_ptr<trace_api::SpanContext>(
      new trace_api::SpanContext(trace_id, span_id, trace_api::TraceFlags(), false,
                                 is_parent_span_valid ? parent_span_context.trace_state()
                                                      : trace_api::TraceState::GetDefault()));

  attributes.ForEachKeyValue(
      [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
        recordable->SetAttribute(key, value);
        return true;
      });

  links.ForEachKeyValue([&](opentelemetry::trace::SpanContext span_context,
                            const opentelemetry::common::KeyValueIterable &attributes) {
    recordable->AddLink(span_context, attributes);
    return true;
  });

  recordable->SetSpanKind(options.kind);
  recordable->SetStartTime(NowOr(options.start_system_time));
  start_steady_time = NowOr(options.start_steady_time);
  exportable_->GetTracer().GetActiveProcessor().OnStart(*exportable_, parent_span_context);
}

Span::~Span()
{
  End();
}

void Span::SetAttribute(nostd::string_view key,
                        const opentelemetry::common::AttributeValue &value) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_) {
    return;
  }
  recordable->SetAttribute(key, value);
}

void Span::AddEvent(nostd::string_view name) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_)
  {
    return;
  }
  recordable->AddEvent(name);
}

void Span::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_)
  {
    return;
  }
  recordable->AddEvent(name, timestamp);
}

void Span::AddEvent(nostd::string_view name,
                    core::SystemTimestamp timestamp,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_)
  {
    return;
  }
  recordable->AddEvent(name, timestamp, attributes);
}

void Span::SetStatus(opentelemetry::trace::StatusCode code, nostd::string_view description) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_)
  {
    return;
  }
  recordable->SetStatus(code, description);
}

void Span::UpdateName(nostd::string_view name) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  auto* recordable = GetRecordable();
  if (recordable == nullptr || has_ended_)
  {
    return;
  }
  recordable->SetName(name);
}

void Span::End(const trace_api::EndSpanOptions &options) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};

  if (has_ended_ == true)
  {
    return;
  }
  has_ended_ = true;

  auto* recordable = GetRecordable();
  if (recordable == nullptr)
  {
    return;
  }

  auto end_steady_time = NowOr(options.end_steady_time);
  recordable->SetDuration(std::chrono::steady_clock::time_point(end_steady_time) -
                           std::chrono::steady_clock::time_point(start_steady_time));
  exportable_->GetTracer().GetActiveProcessor().OnEnd(std::move(exportable_));
}

bool Span::IsRecording() const noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  return GetRecordable() != nullptr;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
