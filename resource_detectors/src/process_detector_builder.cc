// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/resource_detectors/process_detector.h"
#include "opentelemetry/resource_detectors/process_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

void ProcessDetectorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<ProcessDetectorBuilder>();
  registry->SetProcessResourceDetectorBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> ProcessDetectorBuilder::Build(
    const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration * /* model */)
    const
{
  return std::make_unique<ProcessResourceDetector>();
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
