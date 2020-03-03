#include "sdk/src/trace/span.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
Span::Span(std::shared_ptr<Tracer> &&tracer,
           nostd::string_view name,
           const trace_api::StartSpanOptions &options) noexcept
    : tracer_{std::move(tracer)}, recordable_{tracer_->recorder().MakeRecordable()}
{
  (void)name;
  (void)options;
}

void Span::AddEvent(nostd::string_view name) noexcept
{
  (void)name;
}

void Span::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  (void)name;
  (void)timestamp;
}

void Span::AddEvent(nostd::string_view name, core::SteadyTimestamp timestamp) noexcept
{
  (void)name;
  (void)timestamp;
}

void Span::SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept
{
  (void)code;
  (void)description;
}

void Span::UpdateName(nostd::string_view name) noexcept
{
  (void)name;
}

void Span::End() noexcept {}

bool Span::IsRecording() const noexcept
{
  return true;
}
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
