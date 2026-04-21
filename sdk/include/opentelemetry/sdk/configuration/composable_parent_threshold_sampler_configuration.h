// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

// Include the new base class
#include "opentelemetry/sdk/configuration/composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// Inherit from ComposableSamplerConfiguration
class ComposableParentThresholdSamplerConfiguration : public ComposableSamplerConfiguration
{
public:
  ComposableParentThresholdSamplerConfiguration() = default;

  // Enforce strong typing for the root pointer
  std::unique_ptr<ComposableSamplerConfiguration> root;

  void Accept(SamplerConfigurationVisitor *visitor) const override;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
