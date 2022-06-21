// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Builds a TraceIdRatioBasedSampler.
 */
class TraceIdRatioBasedSamplerFactory
{
public:
  static std::unique_ptr<Sampler> Build(double ratio);
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
