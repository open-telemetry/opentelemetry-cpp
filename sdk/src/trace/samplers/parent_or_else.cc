#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
ParentOrElseSampler::ParentOrElseSampler(std::shared_ptr<Sampler> delegate_sampler) noexcept
    : delegate_sampler_(delegate_sampler),
      description_("ParentOrElse{" + std::string{delegate_sampler->GetDescription()} + "}")
{}

SamplingResult ParentOrElseSampler::ShouldSample(
    const trace_api::SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const trace_api::KeyValueIterable &attributes) noexcept
{
  if (parent_context == nullptr)
  {
    // If no parent (root span) exists returns the result of the delegateSampler
    return delegate_sampler_->ShouldSample(parent_context, trace_id, name, span_kind, attributes);
  }
  
  // If parent exists:
  if (parent_context->IsSampled())
  {
    return {Decision::RECORD_AND_SAMPLE, nullptr};
  }

  return {Decision::NOT_RECORD, nullptr};
}

nostd::string_view ParentOrElseSampler::GetDescription() const noexcept
{
  return description_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
