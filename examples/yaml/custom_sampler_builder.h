// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_sampler_builder.h"

class CustomSamplerBuilder : public opentelemetry::sdk::init::ExtensionSamplerBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  CustomSamplerBuilder()           = default;
  ~CustomSamplerBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const opentelemetry::sdk::configuration::ExtensionSamplerConfiguration *model) const override;
};
