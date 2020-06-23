#include "opentelemetry/sdk/trace/always_off_sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

SamplingResult AlwaysOffSampler::ShouldSample(
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const nostd::span<AttributeKeyValue> &attributes) noexcept
  {
    return { Decision::NOT_RECORD, nullptr };
  }

std::string AlwaysOffSampler::GetDescription() const noexcept
{
  return "AlwaysOffSampler";
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE