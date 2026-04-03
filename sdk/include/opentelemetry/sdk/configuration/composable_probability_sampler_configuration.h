// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ComposableProbabilitySamplerConfiguration : public SamplerConfiguration
{
public:
  ComposableProbabilitySamplerConfiguration()           = default;
  ~ComposableProbabilitySamplerConfiguration() override = default;

  // The core property for this sampler type
  double probability{1.0};

  void Accept(SamplerConfigurationVisitor *visitor) const override;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE