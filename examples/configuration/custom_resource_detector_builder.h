// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"

class CustomResourceDetectorBuilder
    : public opentelemetry::sdk::configuration::ExtensionResourceDetectorBuilder
{
public:
  static void Register(opentelemetry::sdk::configuration::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model)
      const override;
};
