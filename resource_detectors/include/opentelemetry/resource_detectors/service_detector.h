// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

/**
 * ServiceResourceDetector to detect resource attributes describing the service
 * the process belongs to. It sets service.name and service.instance.id following
 * the OpenTelemetry semantic conventions.
 *
 * Platform specific behavior and limitations:
 * - service.name is read from the OTEL_SERVICE_NAME environment variable when set.
 * - When OTEL_SERVICE_NAME is not set, service.name falls back to
 *   unknown_service:<process.executable.name> or unknown_service when the
 *   executable name is unavailable.
 * - service.instance.id is a stable RFC 4122 UUID version 4 value for the
 *   current process.
 */
class ServiceResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  opentelemetry::sdk::resource::Resource Detect() noexcept override;
};

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
