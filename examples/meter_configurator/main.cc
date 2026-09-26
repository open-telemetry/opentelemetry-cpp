// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// This example shows how to use MeterProvider::UpdateMeterConfigurator to enable and disable
// meters by instrumentation scope at runtime. Updating the MeterConfigurator affects all existing
// and future meters provided by the MeterProvider. It is safe to call concurrently with
// MeterProvider::GetMeter and with instrument creation and recording on existing instruments.
//
// Three instrumentation scopes are shown:
// 1. "my_application"   (example instrumented user application code),
// 2. "my_library"       (example instrumented user library code),
// 3. "external_library" (example instrumented third-party dependency).
//
// The example simulates a typical cost-management and debugging workflow:
//
// Stage 1: Startup. All meters are enabled and all three scopes report metrics.
// Stage 2: The noisy external_library metrics are not needed in steady state, so its meter is
//          disabled. Its measurements are no longer collected or exported.
// Stage 3: A user reports unexpected behavior, so the external_library meter is re-enabled to
//          investigate. Its metrics are collected and exported again.
// Stage 4: The investigation completes and the external_library meter is disabled again.
//
// Instruments observe their Meter's enabled state, so they need not be recreated after an update.
// Measurements recorded while a Meter is disabled are dropped, not buffered.

#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <utility>

#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/service_attributes.h"

namespace metrics_api       = opentelemetry::metrics;
namespace metrics_sdk       = opentelemetry::sdk::metrics;
namespace metrics_exporters = opentelemetry::exporter::metrics;
namespace scope_sdk         = opentelemetry::sdk::instrumentationscope;
namespace nostd             = opentelemetry::nostd;

namespace
{

// Simulated third-party dependency instrumented under its own scope.
namespace external_library
{
class ExternalModule
{
public:
  ExternalModule()
      : meter_(metrics_api::Provider::GetMeterProvider()->GetMeter("external_library")),
        request_count_(meter_->CreateUInt64Counter("external_library.requests",
                                                   "Requests handled by the external library",
                                                   "{request}"))
  {}

  bool Execute()
  {
    request_count_->Add(1);
    return true;
  }

private:
  nostd::shared_ptr<metrics_api::Meter> meter_;
  nostd::unique_ptr<metrics_api::Counter<std::uint64_t>> request_count_;
};
}  // namespace external_library

// Simulated user library instrumented under its own scope.
namespace my_library
{
class MyModule
{
public:
  MyModule()
      : meter_(metrics_api::Provider::GetMeterProvider()->GetMeter("my_library")),
        call_count_(
            meter_->CreateUInt64Counter("my_library.calls", "Calls into the library", "{call}"))
  {}

  bool Execute()
  {
    call_count_->Add(1);
    return external_module_.Execute();
  }

private:
  nostd::shared_ptr<metrics_api::Meter> meter_;
  nostd::unique_ptr<metrics_api::Counter<std::uint64_t>> call_count_;
  external_library::ExternalModule external_module_;
};
}  // namespace my_library

// Simulated user application instrumented under its own scope.
class MyApplication
{
public:
  MyApplication()
      : meter_(metrics_api::Provider::GetMeterProvider()->GetMeter("my_application")),
        work_count_(meter_->CreateUInt64Counter("my_application.work_items",
                                                "Work items processed by the application",
                                                "{item}"))
  {}

  void Execute()
  {
    work_count_->Add(1);
    my_module_.Execute();
  }

private:
  nostd::shared_ptr<metrics_api::Meter> meter_;
  nostd::unique_ptr<metrics_api::Counter<std::uint64_t>> work_count_;
  my_library::MyModule my_module_;
};

// Builds a configurator that applies default_config to all scopes, with optional per-scope
// overrides.
std::unique_ptr<scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>> MakeMeterConfigurator(
    metrics_sdk::MeterConfig default_config,
    std::initializer_list<std::pair<nostd::string_view, metrics_sdk::MeterConfig>> overrides = {})
{
  scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>::Builder builder(default_config);
  for (const auto &kv : overrides)
  {
    const auto &name   = kv.first;
    const auto &config = kv.second;
    builder.AddConditionNameEquals(name, config);
  }
  return std::make_unique<scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>>(builder.Build());
}

// Creates a MeterProvider with an OStreamMetricExporter and an initial ScopeConfigurator.
std::shared_ptr<metrics_sdk::MeterProvider> CreateMeterProvider(
    std::unique_ptr<scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>> configurator)
{
  auto exporter = metrics_exporters::OStreamMetricExporterFactory::Create();

  // Most exports come from the per-stage ForceFlush. The reader also collects once at startup,
  // so the exact batch count and ordering is not deterministic.
  metrics_sdk::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis = std::chrono::milliseconds(10000);
  options.export_timeout_millis  = std::chrono::milliseconds(500);
  auto reader =
      metrics_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), options);

  auto provider = std::make_shared<metrics_sdk::MeterProvider>(
      std::unique_ptr<metrics_sdk::ViewRegistry>(new metrics_sdk::ViewRegistry()),
      opentelemetry::sdk::resource::Resource::Create(
          {{opentelemetry::semconv::service::kServiceName, "meter_configurator_example"}}),
      std::move(configurator));
  provider->AddMetricReader(std::move(reader));
  return provider;
}

}  // namespace

int main()
{
  const metrics_sdk::MeterConfig enabled_config  = metrics_sdk::MeterConfig::Enabled();
  const metrics_sdk::MeterConfig disabled_config = metrics_sdk::MeterConfig::Disabled();

  // Stage 1: Startup, all meters enabled.
  auto sdk_meter_provider = CreateMeterProvider(MakeMeterConfigurator(enabled_config));

  metrics_api::Provider::SetMeterProvider(
      nostd::shared_ptr<metrics_api::MeterProvider>(sdk_meter_provider));

  // Instantiate the application. This creates the meters and instruments for all three scopes.
  // All meters are enabled, so all three scopes get working instruments.
  MyApplication my_app;

  std::cout << "Stage 1: startup, all meters enabled\n";
  std::cout << "  my_application, my_library and external_library report metrics\n";
  my_app.Execute();
  sdk_meter_provider->ForceFlush();

  // Stage 2: Steady state. Disable the noisy external_library meter.
  std::cout << "\nStage 2: steady state, external_library meter disabled\n";
  std::cout << "  only my_application and my_library report metrics\n";

  sdk_meter_provider->UpdateMeterConfigurator(
      MakeMeterConfigurator(enabled_config, {{"external_library", disabled_config}}));

  my_app.Execute();
  sdk_meter_provider->ForceFlush();

  // Stage 3: A user reports unexpected behavior. Re-enable the external_library meter.
  std::cout << "\nStage 3: investigating, external_library meter re-enabled\n";
  std::cout << "  all three scopes report metrics again\n";

  sdk_meter_provider->UpdateMeterConfigurator(MakeMeterConfigurator(enabled_config));

  my_app.Execute();
  sdk_meter_provider->ForceFlush();

  // Stage 4: Investigation complete. Disable the external_library meter again.
  std::cout << "\nStage 4: investigation complete, external_library meter disabled again\n";
  std::cout << "  only my_application and my_library report metrics\n";

  sdk_meter_provider->UpdateMeterConfigurator(
      MakeMeterConfigurator(enabled_config, {{"external_library", disabled_config}}));

  my_app.Execute();
  sdk_meter_provider->ForceFlush();

  sdk_meter_provider->Shutdown();

  const nostd::shared_ptr<metrics_api::MeterProvider> none;
  metrics_api::Provider::SetMeterProvider(none);
  return 0;
}
