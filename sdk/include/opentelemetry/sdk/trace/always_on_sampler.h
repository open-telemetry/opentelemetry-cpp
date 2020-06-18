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
class AlwaysOnSamplingResult final : public sdk::trace::SamplingResult
{
  Decision GetDecision() const noexcept override;
  nostd::span<std::pair<nostd::string_view, common::AttributeValue>> GetAttributes() const
      noexcept override;
};

class AlwaysOnSampler final : public sdk::trace::Sampler
{
public:
  /**
   * Default constructor
   */
  AlwaysOnSampler() noexcept {}
  /**
   * Returned SamplingResult always has RECORD_AND_SAMPLED
   *    as the Decision
   */
  std::shared_ptr<SamplingResult> ShouldSample(
      const SpanContext *parent_context,
      trace_api::TraceId trace_id,
      nostd::string_view name,
      trace_api::SpanKind span_kind,
      nostd::span<std::pair<nostd::string_view, common::AttributeValue>>
          attributes) noexcept override;

  /**
   * @return Description MUST be AlwaysOnSampler
   */
  std::string GetDescription() const noexcept override;

private:
  opentelemetry::sdk::AtomicSharedPtr<SamplingResult> sampling_result_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
