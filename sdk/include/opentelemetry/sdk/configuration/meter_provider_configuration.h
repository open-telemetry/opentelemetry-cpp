// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// REF: schema/meter_provider.json
class MeterProviderConfiguration
{
public:
  std::vector<std::unique_ptr<MetricReaderConfiguration>> readers;
  std::vector<std::unique_ptr<ViewConfiguration>> views;
  // FIXME: exemplar_filter
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE