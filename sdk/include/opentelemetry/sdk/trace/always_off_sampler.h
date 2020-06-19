#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;
/**
 * The always off sampler always returns NOT_RECORD, effectively disabling
 * tracing functionality.
 */
class AlwaysOffSampler : public Sampler
{
public:
  SamplingResult ShouldSample(
    std::shared_ptr<SpanContext> parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    nostd::span<std::pair<nostd::string_view, common::AttributeValue>> attributes) noexcept override
  {
  	return _result;
  }

  std::string GetDescription() const noexcept override
  {
  	return "AlwaysOffSampler";
  }

private:
	SamplingResult _result = AlwaysOffSamplingResultImpl();
};

class AlwaysOffSamplingResultImpl : public SamplingResult
{
public:
	Decision GetDecision() const noexcept override
	{
		return NOT_RECORD;
	}

	nostd::span<std::pair<nostd::string_view, common::AttributeValue>> GetAttributes() const noexcept override
	{
		return nostd::span();
	}
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE