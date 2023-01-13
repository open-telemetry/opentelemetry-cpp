// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class OPENTELEMETRY_API AggregationConfig
{
public:
  virtual ~AggregationConfig() = default;
};

class OPENTELEMETRY_API HistogramAggregationConfig : public AggregationConfig
{
public:
  std::vector<double> boundaries_;
  bool record_min_max_ = true;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
