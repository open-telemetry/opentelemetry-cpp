// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <chrono>
#include <exception>
#include <mutex>
#include <ostream>
#include <utility>

#include "opentelemetry/common/key_value_iterable.h"  // IWYU pragma: keep
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/export/metric_filter.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace resource    = opentelemetry::sdk::resource;
namespace metrics_api = opentelemetry::metrics;

namespace
{

nostd::shared_ptr<metrics_api::Meter> CreateNoopMeterFallback()
{
  return nostd::shared_ptr<metrics_api::Meter>(new metrics_api::NoopMeter());
}

void LogGetMeterConstructionFailure(const char *detail) noexcept
{
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    OTEL_INTERNAL_LOG_ERROR("[MeterProvider::GetMeter] Failed to construct meter: "
                            << detail << "; returning noop meter.");
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  catch (...)  // NOLINT(bugprone-empty-catch)
  {
    // Logging must not throw from a noexcept GetMeter path.
  }
#endif
}

}  // namespace

MeterProvider::MeterProvider()
    : context_(std::make_shared<MeterContext>(
          std::make_unique<ViewRegistry>(),
          resource::Resource::Create({}),
          std::make_unique<instrumentationscope::ScopeConfigurator<MeterConfig>>(
              instrumentationscope::ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Default())
                  .Build()))),
      noop_meter_(CreateNoopMeterFallback())
{}

MeterProvider::MeterProvider(std::unique_ptr<MeterContext> context)
    : context_(std::move(context)), noop_meter_(CreateNoopMeterFallback())
{}

MeterProvider::MeterProvider(
    std::unique_ptr<ViewRegistry> views,
    const sdk::resource::Resource &resource,
    std::unique_ptr<instrumentationscope::ScopeConfigurator<MeterConfig>> meter_configurator)
    : context_(std::make_shared<MeterContext>(std::move(views),
                                              resource,
                                              std::move(meter_configurator))),
      noop_meter_(CreateNoopMeterFallback())
{
  OTEL_INTERNAL_LOG_DEBUG("[MeterProvider] MeterProvider created.");
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
nostd::shared_ptr<metrics_api::Meter> MeterProvider::GetMeter(
    nostd::string_view name,
    nostd::string_view version,
    nostd::string_view schema_url,
    const opentelemetry::common::KeyValueIterable *attributes) noexcept
#else
nostd::shared_ptr<metrics_api::Meter> MeterProvider::GetMeter(
    nostd::string_view name,
    nostd::string_view version,
    nostd::string_view schema_url) noexcept
#endif
{
#if OPENTELEMETRY_ABI_VERSION_NO < 2
  const opentelemetry::common::KeyValueIterable *attributes = nullptr;
#endif

  if (name.data() == nullptr || name == "")
  {
    OTEL_INTERNAL_LOG_WARN("[MeterProvider::GetMeter] Library name is empty.");
    name = "";
  }

  const std::lock_guard<std::mutex> guard(lock_);

  for (auto &meter : context_->GetMeters())
  {
    auto meter_lib = meter->GetInstrumentationScope();
    if (meter_lib->equal(name, version, schema_url, attributes))
    {
      return nostd::shared_ptr<metrics_api::Meter>{meter};
    }
  }

#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    instrumentationscope::InstrumentationScopeAttributes attrs_map(attributes);
    auto scope =
        instrumentationscope::InstrumentationScope::Create(name, version, schema_url, attrs_map);

    auto meter = std::make_shared<Meter>(context_, std::move(scope));
    context_->AddMeter(meter);
    return nostd::shared_ptr<metrics_api::Meter>{meter};
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  catch (const std::exception &ex)
  {
    LogGetMeterConstructionFailure(ex.what());
    return noop_meter_;
  }
  catch (...)
  {
    LogGetMeterConstructionFailure("unknown exception");
    return noop_meter_;
  }
#endif
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
void MeterProvider::RemoveMeter(nostd::string_view name,
                                nostd::string_view version,
                                nostd::string_view schema_url) noexcept
{
  if (name.data() == nullptr || name == "")
  {
    OTEL_INTERNAL_LOG_WARN("[MeterProvider::RemoveMeter] Library name is empty.");
    name = "";
  }

  const std::lock_guard<std::mutex> guard(lock_);

  context_->RemoveMeter(name, version, schema_url);
}
#endif

const resource::Resource &MeterProvider::GetResource() const noexcept
{
  return context_->GetResource();
}

void MeterProvider::AddMetricReader(std::shared_ptr<MetricReader> reader,
                                    std::unique_ptr<MetricFilter> metric_filter) noexcept
{
  context_->AddMetricReader(std::move(reader), std::move(metric_filter));
}

void MeterProvider::AddView(std::unique_ptr<InstrumentSelector> instrument_selector,
                            std::unique_ptr<MeterSelector> meter_selector,
                            std::unique_ptr<View> view) noexcept
{
  context_->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));
}

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

void MeterProvider::SetExemplarFilter(metrics::ExemplarFilterType exemplar_filter_type) noexcept
{
  context_->SetExemplarFilter(exemplar_filter_type);
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW

/**
 * Shutdown the meter provider.
 */
bool MeterProvider::Shutdown(std::chrono::microseconds timeout) noexcept
{
  // Shutdown only once
  if (shutdown_latch_.test_and_set(std::memory_order_acquire))
  {
    return true;
  }
  return context_->Shutdown(timeout);
}

/**
 * Force flush the meter provider.
 */
bool MeterProvider::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return context_->ForceFlush(timeout);
}

/**
 * Shutdown MeterContext when MeterProvider is destroyed.
 *
 */
MeterProvider::~MeterProvider()
{
  if (context_)
  {
    Shutdown();
  }
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
