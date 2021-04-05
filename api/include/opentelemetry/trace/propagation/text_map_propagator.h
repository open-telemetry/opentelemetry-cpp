#pragma once

#include <cstdint>
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

class TextMapCarrier
{

public:
  virtual nostd::string_view Get(const nostd::string_view &key) const noexcept              = 0;
  virtual void Set(const nostd::string_view &key, const nostd::string_view &value) noexcept = 0;
};

// The TextMapPropagator class provides an interface that enables extracting and injecting
// context into carriers that travel in-band across process boundaries. HTTP frameworks and clients
// can integrate with TextMapPropagator by providing the object containing the
// headers, and a getter and setter function for the extraction and
// injection of values, respectively.

class TextMapPropagator
{
public:
  // Returns the context that is stored in the carrier with the getter as extractor.
  virtual context::Context Extract(const TextMapCarrier &carrier,
                                   context::Context &context) noexcept = 0;

  // Sets the context for carrier with self defined rules.
  virtual void Inject(TextMapCarrier &carrier, const context::Context &context) noexcept = 0;
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
