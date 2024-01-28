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

class ParentBasedSamplerConfiguration : public SamplerConfiguration
{
public:
  std::unique_ptr<SamplerConfiguration> root;
  std::unique_ptr<SamplerConfiguration> remote_parent_sampled;
  std::unique_ptr<SamplerConfiguration> remote_parent_not_sampled;
  std::unique_ptr<SamplerConfiguration> local_parent_sampled;
  std::unique_ptr<SamplerConfiguration> local_parent_not_sampled;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
