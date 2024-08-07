// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/registry.h"
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
    catch (const std::exception &e)
    {
      OTEL_INTERNAL_LOG_ERROR("[Configured Sdk] builder failed with exception: " << e.what());
    }
    catch (...)
    {
      OTEL_INTERNAL_LOG_ERROR("[Configured Sdk] builder failed with unknown exception.");
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

  if (m_meter_provider)
  {
    std::shared_ptr<opentelemetry::metrics::MeterProvider> api_meter_provider = m_meter_provider;
    opentelemetry::metrics::Provider::SetMeterProvider(api_meter_provider);
  }

  if (m_logger_provider)
  {
    std::shared_ptr<opentelemetry::logs::LoggerProvider> api_logger_provider = m_logger_provider;
    opentelemetry::logs::Provider::SetLoggerProvider(api_logger_provider);
  }
}

void ConfiguredSdk::UnInstall()
{
  if (m_tracer_provider)
  {
    std::shared_ptr<opentelemetry::trace::TracerProvider> none;
    opentelemetry::trace::Provider::SetTracerProvider(none);
  }

  if (m_meter_provider)
  {
    std::shared_ptr<opentelemetry::metrics::MeterProvider> none;
    opentelemetry::metrics::Provider::SetMeterProvider(none);
  }

  if (m_logger_provider)
  {
    std::shared_ptr<opentelemetry::logs::LoggerProvider> none;
    opentelemetry::logs::Provider::SetLoggerProvider(none);
  }
}

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
