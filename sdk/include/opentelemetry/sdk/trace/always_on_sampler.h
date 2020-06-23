#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class AlwaysOnSampler final : public sdk::trace::Sampler
{
public:
  /**
   * Returned SamplingResult always has RECORD_AND_SAMPLED
   *    as the Decision
   */
  SamplingResult ShouldSample(
      const SpanContext *parent_context,
      trace_api::TraceId trace_id,
      nostd::string_view name,
      trace_api::SpanKind span_kind,
      const KeyValueIterable &attributes) noexcept override;

  /**
   * @return Description MUST be AlwaysOnSampler
   */
  std::string GetDescription() const noexcept override;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
