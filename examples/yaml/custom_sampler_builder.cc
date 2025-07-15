// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>

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

static CustomSamplerBuilder singleton;

void CustomSamplerBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  registry->AddExtensionSamplerBuilder("my_custom_sampler", &singleton);
}
