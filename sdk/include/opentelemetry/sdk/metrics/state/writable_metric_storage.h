// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class WritableMetricStorage
{
public:
  virtual void Record(T value) noexcept                                             = 0;
  virtual void Record(T value, const common::KeyValueIterable &attributes) noexcept = 0;
};

template <class T>
class NoopWritableMetricsStorage : public WritableMetricStorage<T>
{
public:
  void Record(T value) noexcept override {}
  void Record(T value, const common::KeyValueIterable &attributes) noexcept override {}
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif