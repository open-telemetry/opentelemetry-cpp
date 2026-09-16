// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/resource_detectors/service_detector.h"
#include "opentelemetry/resource_detectors/service_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

void ServiceDetectorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<ServiceDetectorBuilder>();
  registry->SetServiceResourceDetectorBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> ServiceDetectorBuilder::Build(
    const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration * /* model */)
    const
{
  return std::make_unique<ServiceResourceDetector>();
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
