// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <list>
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class AggregationConfig
{
public:
  virtual ~AggregationConfig() = default;
};

template <typename T>
class HistogramAggregationConfig : public AggregationConfig
{
public:
  std::list<T> boundaries_;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_PREVIEW