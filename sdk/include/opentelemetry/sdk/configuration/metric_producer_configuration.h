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
class MetricProducerConfigurationVisitor;

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: MetricProducer
class MetricProducerConfiguration
{
public:
  MetricProducerConfiguration()          = default;
  virtual ~MetricProducerConfiguration() = default;

  virtual void Accept(MetricProducerConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
