#include "opentelemetry/sdk/trace/always_on_sampler.h"

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
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
    const nostd::span<AttributeKeyValue> &attributes) noexcept
{
  return
  {
    sdk::trace::Decision::RECORD_AND_SAMPLE,
        std::unique_ptr<nostd::span<AttributeKeyValue>>(new nostd::span<AttributeKeyValue>())
  };
}

std::string AlwaysOnSampler::GetDescription() const noexcept
{
  return "AlwaysOnSampler";
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
