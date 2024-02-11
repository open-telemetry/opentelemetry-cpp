// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class PropagatorConfigurationVisitor;

class PropagatorConfiguration
{
public:
  PropagatorConfiguration()          = default;
  virtual ~PropagatorConfiguration() = default;

  virtual void Accept(PropagatorConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
