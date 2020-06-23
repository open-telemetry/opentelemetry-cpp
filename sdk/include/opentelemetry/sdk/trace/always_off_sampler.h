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
  /**
   * @return Returns NOT_RECORD always
   */
  SamplingResult ShouldSample(
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const nostd::span<AttributeKeyValue> &attributes) noexcept override;
  
  /**
   * @return Description MUST be AlwaysOffSampler
   */
  std::string GetDescription() const noexcept override;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE