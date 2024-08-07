// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SamplerConfigurationVisitor;

class SamplerConfiguration
{
public:
  SamplerConfiguration()          = default;
  virtual ~SamplerConfiguration() = default;

  virtual void Accept(SamplerConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
