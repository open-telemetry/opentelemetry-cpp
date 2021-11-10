// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/nostd/shared_ptr.h"

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

  ObserverResult(AsynchronousInstrument<T> *instrument) : instrument_(instrument) {}

  virtual void observe(T value, const common::KeyValueIterable &labels)
  {
    instrument_->observe(value, labels);
  }

private:
  AsynchronousInstrument<T> *instrument_;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
