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
 * Builds a AlwaysOffSampler.
 */
class AlwaysOffSamplerFactory
{
public:
  static std::unique_ptr<Sampler> Build();
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
