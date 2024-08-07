// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SimplePropagatorConfiguration : public PropagatorConfiguration
{
public:
  SimplePropagatorConfiguration()           = default;
  ~SimplePropagatorConfiguration() override = default;

  void Accept(PropagatorConfigurationVisitor *visitor) const override
  {
    visitor->VisitSimple(this);
  }

  std::string name;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
