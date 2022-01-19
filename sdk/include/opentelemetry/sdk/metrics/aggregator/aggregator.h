// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregator/accumulation.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class Aggregator
{
public:
  virtual std::unique_ptr<Accumulation> CreateAccumulation() noexcept = 0;

  virtual std::unique_ptr<Accumulation> Merge(Accumulation &prev,
                                              Accumulation &current) noexcept = 0;

  virtual std::unique_ptr<Accumulation> diff(Accumulation &prev,
                                             Accumulation &current) noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif