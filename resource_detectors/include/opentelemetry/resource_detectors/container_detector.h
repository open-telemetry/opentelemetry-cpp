// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace resource
{

/**
 * ContainerResourceDetector to detect resource attributes when running inside a containerized
 * environment. This detector extracts metadata such as container ID from cgroup information and
 * sets attributes like container.id following the OpenTelemetry semantic conventions.
 */
class ContainerResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  opentelemetry::sdk::resource::Resource Detect() noexcept override;
};

}  // namespace resource
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
