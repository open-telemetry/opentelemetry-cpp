// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class Registry;
}  // namespace configuration
}  // namespace sdk

namespace resource_detector
{

class ContainerDetectorBuilder
    : public opentelemetry::sdk::configuration::ContainerResourceDetectorBuilder
{
public:
  static void Register(opentelemetry::sdk::configuration::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model)
      const override;
};

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
