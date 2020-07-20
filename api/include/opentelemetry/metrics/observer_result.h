#pragma once

#include "instrument.h"
#include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/**
 * ObserverResult class is necessary for the callback recording asynchronous
 * instrument use.  Callback functions asynchronous instruments are designed to
 * accept a single ObserverResult object and update using its pointer to the
 * instrument itself.
 */

template <class T>
class ObserverResult
{

public:
  ObserverResult() = default;

  ObserverResult(nostd::shared_ptr<AsynchronousInstrument<T>> instrument) {}

  virtual void observe(T value, const trace::KeyValueIterable &labels) {}

};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
