// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class MetricReaderConfigurationVisitor;

// REF: schema/meter_provider.json
// FIXME: https://github.com/open-telemetry/opentelemetry-configuration/issues/138
class MetricReaderConfiguration
{
public:
  MetricReaderConfiguration()          = default;
  virtual ~MetricReaderConfiguration() = default;

  virtual void Accept(MetricReaderConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
