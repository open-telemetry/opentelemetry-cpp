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

namespace sampler_env
{

/**
 * @brief Returns a sampler built from OTEL_TRACES_SAMPLER and OTEL_TRACES_SAMPLER_ARG
 * environment variables, or a default AlwaysOnSampler if the variables are not set.
 *
 * Supported sampler values:
 * - always_on: Always sample the trace
 * - always_off: Never sample the trace
 * - traceidratio: Sample a fraction of traces based on trace ID, requires OTEL_TRACES_SAMPLER_ARG
 * - parentbased_traceidratio: Sample based on parent and trace ID, requires OTEL_TRACES_SAMPLER_ARG
 * - parentbased_always_on: Use parent sample decision, default to AlwaysOn
 * - parentbased_always_off: Use parent sample decision, default to AlwaysOff
 *
 * For traceidratio and parentbased_traceidratio, OTEL_TRACES_SAMPLER_ARG specifies the ratio
 * (0.0 to 1.0). If unset, defaults to 1.0.
 *
 * If an unknown sampler name or invalid ratio is provided, logs a warning and returns AlwaysOnSampler.
 */
OPENTELEMETRY_EXPORT std::unique_ptr<Sampler> GetSamplerFromEnv();

}  // namespace sampler_env

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE