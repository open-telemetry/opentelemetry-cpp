#pragma once

#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
ParentOrElseSampler::ParentOrElseSampler(std::shared_ptr<Sampler> delegate_sampler) noexcept
    : delegate_sampler_(delegate_sampler)
{}

SamplingResult ParentOrElseSampler::ShouldSample(
    const SpanContext *parent_context,
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
  else
  {
    // If parent exists:
    if (parent_context->sampled_flag)
    {
      return { Decision::RECORD_AND_SAMPLED, nullptr }
    }
    else
    {
      return { Decision::NOT_RECORD, nullptr }
    }
  }
}

std::string ParentOrElseSampler::GetDescription() const noexcept
{
  // use += to concatenate string for speed
  std::string description = "ParentOrElse{";
  description += delegate_sampler_->GetDescription();
  description += "}";
  return description;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE