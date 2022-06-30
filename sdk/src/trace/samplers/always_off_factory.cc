// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"

namespace trace_api = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

std::unique_ptr<Sampler> AlwaysOffSamplerFactory::Build()
{
  std::unique_ptr<Sampler> sampler(new AlwaysOffSampler());
  return std::move(sampler);
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
