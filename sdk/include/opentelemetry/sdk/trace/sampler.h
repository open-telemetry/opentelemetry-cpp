#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/common/attribute_value.h"

#include <memory>
#include <map>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Sampler
{
public:
  /**
   * Initialize a new tracer.
   * @param processor The span processor for this tracer. This must not be a
   * nullptr.
   */
  virtual Decision shouldSample();

  /**
   * Returns the description of this {@code Sampler}. This may be displayed on debug pages or in the
   * logs.
   *
   * <p>Example: "ProbabilitySampler{0.000100}"
   *
   * @return the description of this {@code Sampler}.
   */
  virtual std::string getDescription();
};

class Decision
{
public:
  /**
   * Return sampling decision whether span should be sampled or not.
   *
   * @return sampling decision.
   */
  virtual bool isSampled();

  /**
     * Return tags which will be attached to the span.
     *
     * @return attributes added to span. These attributes should be added to the span only for root
     *     span or when sampling decision {@link #isSampled()} changes from false to true.
     */
  virtual std::map<std::string, common::AttributeValue> getAttributes();
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
