#include "opentelemetry/sdk/trace/always_on_sampler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Return RECORD_AND_SAMPLE and an empty set of attributes
 */
SamplingResult AlwaysOnSampler::ShouldSample(
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const KeyValueIterable &attributes) noexcept
{
  return{sdk::trace::Decision::RECORD_AND_SAMPLE, nullptr};
}

std::string AlwaysOnSampler::GetDescription() const noexcept
{
  return std::string("AlwaysOnSampler");
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
