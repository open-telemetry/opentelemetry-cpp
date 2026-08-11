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
#include "opentelemetry/sdk/configuration/meter_config_configuration.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/meter_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"

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

}  // namespace

void RegisterDefaultMetricsBuilders(Registry *registry)
{
  registry->SetPeriodicMetricReaderBuilder(std::make_unique<DefaultPeriodicMetricReaderBuilder>());
  registry->SetMeterConfiguratorBuilder(std::make_unique<DefaultMeterConfiguratorBuilder>());
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
