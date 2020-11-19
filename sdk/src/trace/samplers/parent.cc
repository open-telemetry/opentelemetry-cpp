#include "opentelemetry/sdk/trace/samplers/parent.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
ParentBasedSampler::ParentBasedSampler(std::shared_ptr<Sampler> delegate_sampler) noexcept
    : delegate_sampler_(delegate_sampler),
      description_("ParentBased{" + std::string{delegate_sampler->GetDescription()} + "}")
{}

SamplingResult ParentBasedSampler::ShouldSample(
    const trace_api::SpanContext &parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const opentelemetry::common::KeyValueIterable &attributes,
    const trace_api::SpanContextKeyValueIterable &links) noexcept
{
  if (!parent_context.IsValid())
  {
    // If no parent (root span) exists returns the result of the delegateSampler
    return delegate_sampler_->ShouldSample(parent_context, trace_id, name, span_kind, attributes,
                                           links);
  }

  // If parent exists:
  if (parent_context.IsSampled())
  {
    return {Decision::RECORD_AND_SAMPLE, nullptr};
  }

  return {Decision::DROP, nullptr};
}

nostd::string_view ParentBasedSampler::GetDescription() const noexcept
{
  return description_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
