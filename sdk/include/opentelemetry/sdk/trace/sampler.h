#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/trace_id.h"
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

/**
 * A sampling Decision for a Span to be created.  
 */
enum class Decision
{
  // IsRecording() == false, span will not be recorded and all events and attributes will be
  // dropped.
  NOT_RECORD,
  // IsRecording() == true, but Sampled flag MUST NOT be set.
  RECORD,
  // IsRecording() == true AND Sampled flag` MUST be set.
  RECORD_AND_SAMPLE
};

/**
 * The output of ShouldSample. 
 * It contains a sampling Decision and a set of Span Attributes.
 */
struct SamplingResult
{
  Decision decision;
  // A set of span Attributes that will also be added to the Span. Can be nullptr.
  std::unique_ptr<const std::map<std::string, opentelemetry::common::AttributeValue>> attributes;
};

/**
 * The Sampler interface allows users to create custom samplers which will return a
 * SamplingResult based on information that is typically available just before the Span was created.
 */
class Sampler
{
public:
  // TODO: Remove this placeholder with real class
  class SpanContext;
  virtual ~Sampler() = default;
  /**
   * Called during Span creation to make a sampling decision.
   *
   * @param parent_context a const pointer of the SpanContext of a parent Span.
   *     null if this is a root span.
   * @param trace_id the TraceId for the new Span. This will be identical to that in
   *     the parentContext, unless this is a root span.
   * @param name the name of the new Span.
   * @param spanKind the trace_api::SpanKind of the Span.
   * @param attributes list of AttributeValue with their keys.
   * @param links TODO: Collection of links that will be associated with the Span to be created.
   * @return sampling result whether span should be sampled or not.
   * @since 0.1.0
   */

  virtual SamplingResult ShouldSample(const SpanContext *parent_context,
                                      trace_api::TraceId trace_id,
                                      nostd::string_view name,
                                      trace_api::SpanKind span_kind,
                                      const KeyValueIterable &attributes) noexcept = 0;

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
