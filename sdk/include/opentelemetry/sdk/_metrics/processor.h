// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/version.h"

#  include <iostream>
#  include <string>
#  include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{

class MetricsProcessor
{
public:
  virtual ~MetricsProcessor() = default;

  virtual std::vector<opentelemetry::sdk::metrics::Record> CheckpointSelf() noexcept = 0;

  virtual void FinishedCollection() noexcept = 0;

  virtual void process(opentelemetry::sdk::metrics::Record record) noexcept = 0;
};

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
