// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class AsynchronousInstrument
{
public:

  /**
   * Sets up a function that will be called whenever a metric collection is initiated.
   */
  virtual void AddCallback(void (*callback)(ObserverResult<T> &, void *), void *state);
  
  /**
   * Sets up a function that will be called whenever a metric collection is initiated.
   */
  virtual void RemoveCallback(void (*callback)(ObserverResult<T> &, void *), void *state);
};

template <class T>
class ObservableCounter : public AsynchronousInstrument
{};

template <class T>
class ObservableGauge : public AsynchronousInstrument
{};

template <class T>
class ObservableUpDownCounter : public AsynchronousInstrument
{};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
