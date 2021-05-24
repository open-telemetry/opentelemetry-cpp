// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/metrics/instrument.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/record.h"
#  include "opentelemetry/version.h"

#  include <iostream>
#  include <string>
#  include <unordered_map>

namespace sdkmetrics = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{

class MetricsProcessor
{
public:
  virtual ~MetricsProcessor() = default;

  virtual std::vector<sdkmetrics::Record> CheckpointSelf() noexcept = 0;

  virtual void FinishedCollection() noexcept = 0;

  virtual void process(sdkmetrics::Record record) noexcept = 0;
};

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
