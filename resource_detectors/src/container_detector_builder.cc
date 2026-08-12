// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/resource_detectors/container_detector.h"
#include "opentelemetry/resource_detectors/container_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

void ContainerDetectorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<ContainerDetectorBuilder>();
  registry->SetContainerResourceDetectorBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> ContainerDetectorBuilder::Build(
    const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration * /* model */)
    const
{
  return std::make_unique<ContainerResourceDetector>();
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
