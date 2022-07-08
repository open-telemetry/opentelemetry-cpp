// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

// typedef  void (*ObservableCallbackPtr)(ObserverResult &, void *);
using ObservableCallbackPtr = void (*)(ObserverResult &, void *);

class ObservableInstrument
{
public:
  /**
   * Sets up a function that will be called whenever a metric collection is initiated.
   */
  virtual void AddCallback(ObservableCallbackPtr, void *state);

  /**
   * Remove a function that was configured to be called whenever a metric collection is initiated.
   */
  virtual void RemoveCallback(ObservableCallbackPtr, void *state);
};

template <class T>
class ObservableCounter : public ObservableInstrument
{};

template <class T>
class ObservableGauge : public ObservableInstrument
{};

template <class T>
class ObservableUpDownCounter : public ObservableInstrument
{};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
