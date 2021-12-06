// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE

#  include <memory>
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

namespace sdk
{
namespace metrics
{

template <class T>
class Aggregation
{
public:
  virtual ~Aggregation() = default;
  virtual std::shared_ptr<Aggregator> CreateAggregator(
      InstrumentDescriptor<T> instrumentDescriptor) = 0;
};

class DefaultAggregation : public Aggregation
{};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif