// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/init/extension_sampler_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/sampler.h"

class CustomSamplerBuilder : public opentelemetry::sdk::init::ExtensionSamplerBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const override;
};
