// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW
#  include "c

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics_new
{

/**
 * ObserverResult class is necessary for the callback recording asynchronous
 * instrument use.
 */

template <class T>
class ObserverResult
{

public:
  virtual void observe(T value) noexcept = 0;

  virtual void observer(T value, const common::KeyValueIterable &labels) noexcept = 0;
};

}  // namespace metrics_new
OPENTELEMETRY_END_NAMESPACE
#endif
