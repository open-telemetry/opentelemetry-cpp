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
  std::unique_ptr<ConfiguredSdk> sdk;

  if (model)
  {
    try
    {
      SdkBuilder builder(registry);
      sdk = builder.CreateConfiguredSdk(model);
    }
    catch (...)
    {
      OTEL_INTERNAL_LOG_ERROR("Failed to build from the model.");
    }
  }

  return sdk;
}

void ConfiguredSdk::Install()
{
  if (m_tracer_provider)
  {
    std::shared_ptr<opentelemetry::trace::TracerProvider> api_tracer_provider = m_tracer_provider;
    opentelemetry::trace::Provider::SetTracerProvider(api_tracer_provider);
  }
}

void ConfiguredSdk::UnInstall()
{
  if (m_tracer_provider)
  {
    std::shared_ptr<opentelemetry::trace::TracerProvider> none;
    opentelemetry::trace::Provider::SetTracerProvider(none);
  }
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
