// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

/**
 * Resolves the logical service name for the service resource detector.
 * Platform-specific behavior:
 *   - Reads OTEL_SERVICE_NAME when set.
 *   - Otherwise falls back to unknown_service:<process.executable.name> when the
 *     executable name is available for the current process.
 *   - Otherwise returns unknown_service.
 */
std::string GetServiceName();

/**
 * Returns a stable service.instance.id for the current process.
 * A new RFC 4122 UUID version 4 value is generated when the process ID changes.
 */
std::string GenerateServiceInstanceId() noexcept;

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
