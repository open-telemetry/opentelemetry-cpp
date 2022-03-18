// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MeterContext::MeterContext(std::vector<std::unique_ptr<MetricExporter>> &&exporters,
                           std::unique_ptr<ViewRegistry> views,
                           opentelemetry::sdk::resource::Resource resource) noexcept
    : resource_{resource},
      exporters_(std::move(exporters)),
      views_(std::move(views)),
      sdk_start_ts_{std::chrono::system_clock::now()}
{}

const resource::Resource &MeterContext::GetResource() const noexcept
{
  return resource_;
}

ViewRegistry *MeterContext::GetViewRegistry() const noexcept
{
  return views_.get();
}

nostd::span<std::shared_ptr<Meter>> MeterContext::GetMeters() noexcept
{
  return nostd::span<std::shared_ptr<Meter>>{meters_};
}

nostd::span<std::shared_ptr<CollectorHandle>> MeterContext::GetCollectors() noexcept
{
  return nostd::span<std::shared_ptr<CollectorHandle>>(collectors_);
}

opentelemetry::common::SystemTimestamp MeterContext::GetSDKStartTime() noexcept
{
  return sdk_start_ts_;
}

void MeterContext::AddMetricExporter(std::unique_ptr<MetricExporter> exporter) noexcept
{
  exporters_.push_back(std::move(exporter));
}

void MeterContext::AddMetricReader(std::unique_ptr<MetricReader> reader) noexcept
{
  auto collector =
      std::shared_ptr<MetricCollector>{new MetricCollector(shared_from_this(), std::move(reader))};
  collectors_.push_back(collector);
}

void MeterContext::AddView(std::unique_ptr<InstrumentSelector> instrument_selector,
                           std::unique_ptr<MeterSelector> meter_selector,
                           std::unique_ptr<View> view) noexcept
{
  views_->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));
}

void MeterContext::AddMeter(std::shared_ptr<Meter> meter)
{
  meters_.push_back(meter);
}

bool MeterContext::Shutdown() noexcept
{
  bool return_status = true;
  if (!shutdown_latch_.test_and_set(std::memory_order_acquire))
  {
    bool result_exporter  = true;
    bool result_reader    = true;
    bool result_collector = true;

    for (auto &exporter : exporters_)
    {
      bool status     = exporter->Shutdown();
      result_exporter = result_exporter && status;
    }
    if (!result_exporter)
    {
      OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Unable to shutdown all metric exporters");
    }
    for (auto &collector : collectors_)
    {
      bool status      = std::static_pointer_cast<MetricCollector>(collector)->Shutdown();
      result_collector = result_reader && status;
    }
    if (!result_collector)
    {
      OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Unable to shutdown all metric readers");
    }
    return_status = result_exporter && result_collector;
  }
  return return_status;
}

bool MeterContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  // TODO - Implement timeout logic.
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(forceflush_lock_);
  bool result_exporter = true;
  for (auto &exporter : exporters_)
  {
    bool status     = exporter->ForceFlush(timeout);
    result_exporter = result_exporter && status;
  }
  if (!result_exporter)
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::ForceFlush] Unable to force-flush all metric exporters");
  }
  return result_exporter;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
