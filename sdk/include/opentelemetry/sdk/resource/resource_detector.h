// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

/**
 * Interface for a Resource Detector
 */
class ResourceDetector
{
public:
  ResourceDetector()          = default;
  virtual ~ResourceDetector() = default;
  virtual Resource Detect()   = 0;

protected:
  static Resource Create(const ResourceAttributes &attributes,
                         const std::string &schema_url = std::string{});
};

/**
 * OTelResourceDetector to detect the presence of and create a Resource
 * from the OTEL_RESOURCE_ATTRIBUTES environment variable.
 */
class OTELResourceDetector : public ResourceDetector
{
public:
  Resource Detect() noexcept override;
};

/**
 * ContainerResourceDetector to detect resource attributes when running inside a containerized environment.
 * This detector extracts metadata such as container ID from cgroup information
 * and sets attributes like container.id following the OpenTelemetry semantic conventions.
 */
class ContainerResourceDetector : public ResourceDetector
{
public:
  Resource Detect() noexcept override;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
