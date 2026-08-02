// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include <ostream>
#  include <string>
#endif

#include <vector>
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/common/env_variables.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
namespace
{

constexpr char kMetricsExemplarFilterEnv[] = "OTEL_METRICS_EXEMPLAR_FILTER";

ExemplarFilterType GetExemplarFilterFromEnvironment()
{
  std::string value;
  if (!common::GetStringEnvironmentVariable(kMetricsExemplarFilterEnv, value) || value.empty())
  {
    return ExemplarFilterType::kTraceBased;
  }

  if (InstrumentDescriptorUtil::CaseInsensitiveAsciiEquals(value, "always_on"))
  {
    return ExemplarFilterType::kAlwaysOn;
  }

  if (InstrumentDescriptorUtil::CaseInsensitiveAsciiEquals(value, "always_off"))
  {
    return ExemplarFilterType::kAlwaysOff;
  }

  if (InstrumentDescriptorUtil::CaseInsensitiveAsciiEquals(value, "trace_based"))
  {
    return ExemplarFilterType::kTraceBased;
  }

  OTEL_INTERNAL_LOG_WARN("Environment variable <" << kMetricsExemplarFilterEnv
                                                  << "> has an invalid value <" << value
                                                  << ">, ignoring");
  return ExemplarFilterType::kTraceBased;
}

}  // namespace
#endif

std::unique_ptr<MeterContext> MeterContextFactory::Create()
{
  auto views = ViewRegistryFactory::Create();
  return Create(std::move(views));
}

std::unique_ptr<MeterContext> MeterContextFactory::Create(std::unique_ptr<ViewRegistry> views)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(views), resource);
}

std::unique_ptr<MeterContext> MeterContextFactory::Create(
    std::unique_ptr<ViewRegistry> views,
    const opentelemetry::sdk::resource::Resource &resource)
{
  auto meter_configurator = std::make_unique<instrumentationscope::ScopeConfigurator<MeterConfig>>(
      instrumentationscope::ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Default())
          .Build());
  return Create(std::move(views), resource, std::move(meter_configurator));
}

std::unique_ptr<MeterContext> MeterContextFactory::Create(
    std::unique_ptr<ViewRegistry> views,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<instrumentationscope::ScopeConfigurator<MeterConfig>> meter_configurator)
{
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
  return Create(std::move(views), resource, std::move(meter_configurator),
                GetExemplarFilterFromEnvironment());
#else
  std::unique_ptr<MeterContext> context(
      new MeterContext(std::move(views), resource, std::move(meter_configurator)));
  return context;
#endif
}

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
std::unique_ptr<MeterContext> MeterContextFactory::Create(
    std::unique_ptr<ViewRegistry> views,
    const opentelemetry::sdk::resource::Resource &resource,
    std::unique_ptr<instrumentationscope::ScopeConfigurator<MeterConfig>> meter_configurator,
    ExemplarFilterType exemplar_filter_type)
{
  std::unique_ptr<MeterContext> context(new MeterContext(
      std::move(views), resource, std::move(meter_configurator), exemplar_filter_type));
  return context;
}
#endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
