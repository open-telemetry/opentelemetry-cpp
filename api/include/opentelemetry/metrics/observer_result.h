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
class ObserverResult
{

public:
  ObserverResult() = default;

  ObserverResult(nostd::shared_ptr<AsynchronousInstrument> instrument) : instrument_(instrument) {}

  void observe(nostd::variant<int, double> value, const trace::KeyValueIterable &labels)
  {
    instrument_->update(value, labels);
  }

private:
  nostd::shared_ptr<AsynchronousInstrument> instrument_;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE