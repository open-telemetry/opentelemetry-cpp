// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <list>
#include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class AggregationConfig
{
public:
  virtual ~AggregationConfig() = default;
};

class HistogramAggregationConfig : public AggregationConfig
{
public:
  std::list<double> boundaries_;
  bool record_min_max_ = true;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
