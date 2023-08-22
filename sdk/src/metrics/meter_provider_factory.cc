// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk_config.h"
#include "opentelemetry/version.h"

#include <vector>

namespace resource    = opentelemetry::sdk::resource;
namespace metrics_api = opentelemetry::metrics;
namespace metrics_sdk = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<opentelemetry::metrics::MeterProvider> MeterProviderFactory::Create()
{
  auto views = ViewRegistryFactory::Create();
  return Create(std::move(views));
}

std::unique_ptr<opentelemetry::metrics::MeterProvider> MeterProviderFactory::Create(
    std::unique_ptr<ViewRegistry> views)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(views), resource);
}

std::unique_ptr<opentelemetry::metrics::MeterProvider> MeterProviderFactory::Create(
    std::unique_ptr<ViewRegistry> views,
    const opentelemetry::sdk::resource::Resource &resource)
{
  std::unique_ptr<opentelemetry::metrics::MeterProvider> provider(
      new metrics_sdk::MeterProvider(std::move(views), resource));
  return provider;
}

std::unique_ptr<opentelemetry::metrics::MeterProvider> MeterProviderFactory::Create(
    std::unique_ptr<sdk::metrics::MeterContext> context)
{
  std::unique_ptr<opentelemetry::metrics::MeterProvider> provider(
      new metrics_sdk::MeterProvider(std::move(context)));
  return provider;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
