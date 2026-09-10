// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/metrics_builders.h"

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/configuration/exemplar_filter.h"
#include "opentelemetry/sdk/configuration/meter_config_configuration.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/meter_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metrics_builder_utils.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

class DefaultPeriodicMetricReaderBuilder : public PeriodicMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const PeriodicMetricReaderConfiguration *model,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;
    options.export_interval_millis = std::chrono::milliseconds(model->interval);
    options.export_timeout_millis  = std::chrono::milliseconds(model->timeout);
    return opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(
        std::move(exporter), options);
  }
};

class DefaultMeterConfiguratorBuilder : public MeterConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::metrics::MeterConfig>>
  Build(const MeterConfiguratorConfiguration *model) const override
  {
    using common::WildcardMatch;
    using opentelemetry::sdk::instrumentationscope::InstrumentationScope;
    using opentelemetry::sdk::instrumentationscope::ScopeConfigurator;
    using opentelemetry::sdk::metrics::MeterConfig;

    MeterConfig default_config =
        model->default_config.enabled ? MeterConfig::Enabled() : MeterConfig::Disabled();
    auto builder = ScopeConfigurator<MeterConfig>::Builder(default_config);

    for (const auto &entry : model->meters)
    {
      MeterConfig entry_config =
          entry.config.enabled ? MeterConfig::Enabled() : MeterConfig::Disabled();
      std::string pattern = entry.name;
      builder.AddCondition(
          [pattern](const InstrumentationScope &scope) {
            return WildcardMatch(pattern, scope.GetName());
          },
          entry_config);
    }

    return std::make_unique<ScopeConfigurator<MeterConfig>>(builder.Build());
  }
};

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
static opentelemetry::sdk::metrics::ExemplarFilterType ConvertExemplarFilter(
    enum opentelemetry::sdk::configuration::ExemplarFilter config)
{
  opentelemetry::sdk::metrics::ExemplarFilterType sdk{
      opentelemetry::sdk::metrics::ExemplarFilterType::kTraceBased};

  switch (config)
  {
    case opentelemetry::sdk::configuration::ExemplarFilter::always_on:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kAlwaysOn;
      break;
    case opentelemetry::sdk::configuration::ExemplarFilter::always_off:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kAlwaysOff;
      break;
    case opentelemetry::sdk::configuration::ExemplarFilter::trace_based:
      sdk = opentelemetry::sdk::metrics::ExemplarFilterType::kTraceBased;
      break;
    default:
      break;
  }

  return sdk;
}
#endif /* ENABLE_METRICS_EXEMPLAR_PREVIEW */

class DefaultMeterProviderBuilder : public MeterProviderBuilder
{
public:
  std::shared_ptr<opentelemetry::sdk::metrics::MeterProvider> Build(
      const MeterProviderBuilderContext &context,
      const opentelemetry::sdk::configuration::MeterProviderConfiguration *model) const override
  {
    if (context.registry == nullptr || context.resource == nullptr)
    {
      static std::string message =
          "MeterProviderBuilderContext must have non-null registry and resource.";
      throw UnsupportedException(message);
    }

    auto view_registry = opentelemetry::sdk::metrics::ViewRegistryFactory::Create();

    for (const auto &view_configuration : model->views)
    {
      MetricsBuilderUtils::AddView(view_registry.get(), view_configuration);
    }

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    auto sdk_exemplar_filter = ConvertExemplarFilter(model->exemplar_filter);
#endif

    std::unique_ptr<opentelemetry::sdk::metrics::MeterContext> meter_context;
    if (model->meter_configurator)
    {
      auto meter_configurator =
          MetricsBuilderUtils::CreateMeterConfigurator(context.registry, model->meter_configurator);
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
          std::move(view_registry), *context.resource, std::move(meter_configurator),
          sdk_exemplar_filter);
#else
      meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
          std::move(view_registry), *context.resource, std::move(meter_configurator));
#endif
    }
    else
    {
      auto default_model =
          std::make_unique<opentelemetry::sdk::configuration::MeterConfiguratorConfiguration>();
      auto meter_configurator =
          MetricsBuilderUtils::CreateMeterConfigurator(context.registry, default_model);
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
          std::move(view_registry), *context.resource, std::move(meter_configurator),
          sdk_exemplar_filter);
#else
      meter_context = opentelemetry::sdk::metrics::MeterContextFactory::Create(
          std::move(view_registry), *context.resource, std::move(meter_configurator));
#endif
    }

    for (const auto &reader_configuration : model->readers)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::MetricReader> metric_reader =
          MetricsBuilderUtils::CreateMetricReader(context.registry, reader_configuration);
      meter_context->AddMetricReader(metric_reader);
    }

#ifndef ENABLE_METRICS_EXEMPLAR_PREVIEW
    /* Do not spam with warnings if disabled anyway. */
    if (model->exemplar_filter != opentelemetry::sdk::configuration::ExemplarFilter::always_off)
    {
      OTEL_INTERNAL_LOG_WARN("ENABLE_METRICS_EXEMPLAR_PREVIEW not set, ignoring exemplar filter");
    }
#endif /* ENABLE_METRICS_EXEMPLAR_PREVIEW */

    return opentelemetry::sdk::metrics::MeterProviderFactory::Create(std::move(meter_context));
  }
};

}  // namespace

void RegisterDefaultMetricsBuilders(Registry *registry)
{
  registry->SetPeriodicMetricReaderBuilder(std::make_unique<DefaultPeriodicMetricReaderBuilder>());
  registry->SetMeterConfiguratorBuilder(std::make_unique<DefaultMeterConfiguratorBuilder>());
  registry->SetMeterProviderBuilder(std::make_unique<DefaultMeterProviderBuilder>());
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
