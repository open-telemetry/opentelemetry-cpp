// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class AlwaysOffSamplerConfiguration : public SamplerConfiguration
{
public:
  AlwaysOffSamplerConfiguration()           = default;
  ~AlwaysOffSamplerConfiguration() override = default;

  void Accept(SamplerConfigurationVisitor *visitor) const override
  {
    visitor->VisitAlwaysOff(this);
  }
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE