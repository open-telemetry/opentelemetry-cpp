// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#include "opentelemetry/sdk/configuration/composable_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration_visitor.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
void ComposableSamplerConfiguration::Accept(SamplerConfigurationVisitor *visitor) const
{
  visitor->VisitComposable(this);
}
}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
