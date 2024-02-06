// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/sdk_builder.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

std::unique_ptr<ConfiguredSdk> ConfiguredSdk::Create(
    std::shared_ptr<Registry> registry,
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model)
{
  SdkBuilder builder(registry);
  return builder.CreateConfiguredSdk(model);
}

void ConfiguredSdk::Install()
{
  opentelemetry::trace::Provider::SetTracerProvider(m_tracer_provider);
}

void ConfiguredSdk::UnInstall()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  opentelemetry::trace::Provider::SetTracerProvider(none);
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
