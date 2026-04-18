// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <memory>
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class ComposableParentThresholdSamplerConfiguration : public SamplerConfiguration
{
public:
  ComposableParentThresholdSamplerConfiguration() = default;
  std::unique_ptr<SamplerConfiguration> root;
  void Accept(SamplerConfigurationVisitor *visitor) const override;
};
}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
