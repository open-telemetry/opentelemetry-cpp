// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"

#include "custom_resource_detector.h"
#include "custom_resource_detector_builder.h"

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
CustomResourceDetectorBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomResourceDetector>(comment);

  return sdk;
}

void CustomResourceDetectorBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomResourceDetectorBuilder>();
  registry->SetExtensionResourceDetectorBuilder("my_custom_detector", std::move(builder));
}
