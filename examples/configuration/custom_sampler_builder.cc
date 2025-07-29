// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <utility>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/registry.h"

#include "custom_sampler.h"
#include "custom_sampler_builder.h"

std::unique_ptr<opentelemetry::sdk::trace::Sampler> CustomSamplerBuilder::Build(
    const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const
{
  // Read yaml attributes
  std::string comment = model->node->GetRequiredString("comment");

  auto sdk = std::make_unique<CustomSampler>(comment);

  return sdk;
}

void CustomSamplerBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<CustomSamplerBuilder>();
  registry->SetExtensionSamplerBuilder("my_custom_sampler", std::move(builder));
}
