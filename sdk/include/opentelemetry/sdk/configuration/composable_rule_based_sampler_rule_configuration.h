// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_patterns_configuration.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_rule_attribute_values_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class ComposableRuleBasedSamplerRuleConfiguration
{
public:
  ComposableRuleBasedSamplerRuleConfiguration() = default;

  std::unique_ptr<ComposableRuleBasedSamplerRuleAttributeValuesConfiguration> attribute_values;
  std::unique_ptr<ComposableRuleBasedSamplerRuleAttributePatternsConfiguration> attribute_patterns;
  std::vector<std::string> parent;
  std::vector<std::string> span_kinds;
  std::unique_ptr<SamplerConfiguration> sampler;
};
}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
