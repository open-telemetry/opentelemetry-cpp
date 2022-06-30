// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Builds a ParentBasedSampler.
 */
class ParentBasedSamplerFactory
{
public:
  static std::unique_ptr<Sampler> Build(std::shared_ptr<Sampler> delegate_sampler);
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
