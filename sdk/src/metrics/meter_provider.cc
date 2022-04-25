// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"

#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace resource    = opentelemetry::sdk::resource;
namespace metrics_api = opentelemetry::metrics;

MeterProvider::MeterProvider(std::shared_ptr<MeterContext> context) noexcept : context_{context} {}

MeterProvider::MeterProvider(std::unique_ptr<ViewRegistry> views,
                             sdk::resource::Resource resource) noexcept
    : context_(std::make_shared<MeterContext>(std::move(views), resource))
{}

nostd::shared_ptr<metrics_api::Meter> MeterProvider::GetMeter(
    nostd::string_view name,
    nostd::string_view version,
    nostd::string_view schema_url) noexcept
{
  if (name.data() == nullptr || name == "")
  {
    OTEL_INTERNAL_LOG_WARN("[MeterProvider::GetMeter] Library name is empty.");
    name = "";
  }

  const std::lock_guard<std::mutex> guard(lock_);

  for (auto &meter : context_->GetMeters())
  {
    auto meter_lib = meter->GetInstrumentationLibrary();
    if (meter_lib->equal(name, version, schema_url))
    {
      return nostd::shared_ptr<metrics_api::Meter>{meter};
    }
  }
  auto lib   = instrumentationlibrary::InstrumentationLibrary::Create(name, version, schema_url);
  auto meter = std::shared_ptr<Meter>(new Meter(context_, std::move(lib)));
  context_->AddMeter(meter);
  return nostd::shared_ptr<metrics_api::Meter>{meter};
}

const resource::Resource &MeterProvider::GetResource() const noexcept
{
  return context_->GetResource();
}

void MeterProvider::AddMetricReader(std::unique_ptr<MetricReader> reader) noexcept
{
  return context_->AddMetricReader(std::move(reader));
}

void MeterProvider::AddView(std::unique_ptr<InstrumentSelector> instrument_selector,
                            std::unique_ptr<MeterSelector> meter_selector,
                            std::unique_ptr<View> view) noexcept
{
  return context_->AddView(std::move(instrument_selector), std::move(meter_selector),
                           std::move(view));
}

/**
 * Shutdown the meter provider.
 */
bool MeterProvider::Shutdown() noexcept
{
  return context_->Shutdown();
}

/**
 * Force flush the meter provider.
 */
bool MeterProvider::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return context_->ForceFlush(timeout);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
