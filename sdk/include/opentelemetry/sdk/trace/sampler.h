#pragma once

#include "opentelemetry/common/attribute_value.h"
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
   * @param parent_context TODO: the parent span's SpanContext. null if this is a root
   *     span.
   * @param traceId the TraceId for the new Span. This will be identical to that in
   *     the parentContext, unless this is a root span.
   * @param name the name of the new Span.
   * @param parentLinks TODO: the parentLinks associated with the new Span.
   * @param spanKind the trace_api::SpanKind of the Span.
   * @param attributes TODO: current map is a placeholder.
   *     list of AttributeValue with their keys.
   * @return sampling decision whether span should be sampled or not.
   * @since 0.1.0
   */

  virtual SamplingResult ShouldSample(
      std::shared_ptr<SpanContext> parent_context,
      trace_api::TraceId trace_id,
      nostd::string_view name,
      trace_api::SpanKind span_kind,
      std::map<std::string, common::AttributeValue> attributes) noexcept = 0;

  /**
   * Returns the description of this {@code Sampler}. This may be displayed on debug pages or in the
   * logs.
   *
   * <p>Example: "ProbabilitySampler{0.000100}"
   *
   * @return the description of this {@code Sampler}.
   */
  virtual std::string GetDescription() const noexcept = 0;
};

class SamplingResult
{
private:
  enum Decision
  {
    NOT_RECORD,
    RECORD,
    RECORD_AND_SAMPLE
  };

public:
  // TODO: add Decision enum
  virtual ~SamplingResult() = default;
  /**
   * Return sampling decision whether span should be sampled or not.
   *
   * @return TODO: Decision.
   */
  virtual Decision GetDecision() const noexcept = 0;

  /**
   * Return attributes which will be attached to the span.
   *
   * TODO: Change return value to nostd::span<std::pair<nostd::string_view, common::AttributeValue>>
   * @return attributes added to span. These attributes should be added to the span only for root
   *     span or when sampling decision isSampled() changes from false to true.
   */
  virtual std::map<std::string, common::AttributeValue> GetAttributes() const noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
