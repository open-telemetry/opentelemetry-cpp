// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/composable_probability_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/composable_sampler_configuration_visitor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
void ComposableProbabilitySamplerConfiguration::Accept(
    ComposableSamplerConfigurationVisitor *visitor) const
{
  visitor->VisitComposableProbability(this);
}
}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
