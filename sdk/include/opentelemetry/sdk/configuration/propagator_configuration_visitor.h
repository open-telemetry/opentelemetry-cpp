// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SimplePropagatorConfiguration;
class CompositePropagatorConfiguration;

class PropagatorConfigurationVisitor
{
public:
  PropagatorConfigurationVisitor()          = default;
  virtual ~PropagatorConfigurationVisitor() = default;

  virtual void VisitSimple(const SimplePropagatorConfiguration *model)       = 0;
  virtual void VisitComposite(const CompositePropagatorConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
