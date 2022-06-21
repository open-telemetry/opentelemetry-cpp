// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/samplers/parent.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"

namespace trace_api = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

std::unique_ptr<Sampler> ParentBasedSamplerFactory::Build(std::shared_ptr<Sampler> delegate_sampler)
{
  std::unique_ptr<Sampler> sampler(new ParentBasedSampler(delegate_sampler));
  return sampler;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
