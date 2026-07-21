// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/**
 * Factory class for ProbabilitySampler.
 */
class ProbabilitySamplerFactory
{
public:
  /**
   * Create a ProbabilitySampler.
   * @param ratio the sampling probability, in range [0.0, 1.0]. Values
   * outside the range (including NaN) log a warning and fall back to the
   * default of 1.0.
   */
  static std::unique_ptr<Sampler> Create(double ratio);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
