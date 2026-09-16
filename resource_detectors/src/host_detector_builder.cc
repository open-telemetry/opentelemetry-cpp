// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/resource_detectors/host_detector.h"
#include "opentelemetry/resource_detectors/host_detector_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

void HostDetectorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<HostDetectorBuilder>();
  registry->SetHostResourceDetectorBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> HostDetectorBuilder::Build(
    const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration * /* model */) const
{
  return std::make_unique<HostResourceDetector>();
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
