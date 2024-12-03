// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/propagator.json
// YAML-NODE: Propagator
class PropagatorConfiguration
{
public:
  PropagatorConfiguration()          = default;
  virtual ~PropagatorConfiguration() = default;

  std::vector<std::string> composite;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
