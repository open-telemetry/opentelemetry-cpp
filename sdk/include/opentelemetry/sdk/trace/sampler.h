#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

#include <map>
#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;
class SamplingResult
{
public:
  /**
   * NOT_RECORD - IsRecording() == false, 
   *    span will not be recorded and all events and attributes will be dropped.
   * RECORD - IsRecording() == true, but Sampled flag MUST NOT be set.
   * RECORD_AND_SAMPLED - IsRecording() == true AND Sampled flag` MUST be set.
   */
  enum Decision
  {
    NOT_RECORD,
    RECORD,
    RECORD_AND_SAMPLE
  };
  virtual ~SamplingResult() = default;
  /**
   * Return sampling decision.
   *
   * @return Decision.
   */
  virtual Decision GetDecision() const noexcept = 0;

  /**
   * Return attributes which will be attached to the span.
   *
   * @return the immutable list of attributes added to span.
   */
  virtual nostd::span<std::pair<nostd::string_view, common::AttributeValue>> GetAttributes() const
      noexcept = 0;
};

class Sampler
{
private:
  // Placeholder
  class SpanContext
  {};

public:
  virtual ~Sampler() = default;
  /**
   * Called during Span creation to make a sampling decision.
   *
   * @param parent_context TODO: a shared pointer of the SpanContext of a parent Span. 
   *     null if this is a root span.
   * @param trace_id the TraceId for the new Span. This will be identical to that in
   *     the parentContext, unless this is a root span.
   * @param name the name of the new Span.
   * @param spanKind the trace_api::SpanKind of the Span.
   * @param attributes TODO: current map is a placeholder.
   *     list of AttributeValue with their keys.
   * @param links TODO: Collection of links that will be associated with the Span to be created.
   * @return sampling result whether span should be sampled or not.
   * @since 0.1.0
   */

  virtual std::shared_ptr<SamplingResult> ShouldSample(
      const SpanContext* parent_context,
      trace_api::TraceId trace_id,
      nostd::string_view name,
      trace_api::SpanKind span_kind,
      nostd::span<std::pair<nostd::string_view, common::AttributeValue>> attributes) noexcept = 0;

  /**
   * Returns the sampler name or short description with the configuration. 
   * This may be displayed on debug pages or in the logs.
   *
   * @return the description of this Sampler.
   */
  virtual std::string GetDescription() const noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
