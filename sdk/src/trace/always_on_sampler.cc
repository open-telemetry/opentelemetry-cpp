#include "opentelemetry/sdk/trace/always_on_sampler.h"

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
AlwaysOnSamplingResult::Decision AlwaysOnSamplingResult::GetDecision() const noexcept
{
  return 
}

AlwaysOnSampler::AlwaysOnSampler() noexcept
    : sampling_result_(std::shared_ptr<SamplingResult>(new SamplingResult))
{}
std::shared_ptr<SamplingResult> AlwaysOnSampler::ShouldSample(
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    nostd::span<std::pair<nostd::string_view, common::AttributeValue>> attributes) noexcept
{}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
