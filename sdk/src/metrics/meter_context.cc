// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MeterContext::MeterContext(std::unique_ptr<ViewRegistry> views,
                           opentelemetry::sdk::resource::Resource resource) noexcept
    : resource_{resource}, views_(std::move(views)), sdk_start_ts_{std::chrono::system_clock::now()}
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
  bool result = true;
  if (!shutdown_latch_.test_and_set(std::memory_order_acquire))
  {

    for (auto &collector : collectors_)
    {
      bool status = std::static_pointer_cast<MetricCollector>(collector)->Shutdown();
      result      = result && status;
    }
    if (!result)
    {
      OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Unable to shutdown all metric readers");
    }
  }
  return result;
}

bool MeterContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  // TODO - Implement timeout logic.
  bool result = true;
  if (!shutdown_latch_.test_and_set(std::memory_order_acquire))
  {

    for (auto &collector : collectors_)
    {
      bool status = std::static_pointer_cast<MetricCollector>(collector)->ForceFlush(timeout);
      result      = result && status;
    }
    if (!result)
    {
      OTEL_INTERNAL_LOG_WARN("[MeterContext::ForceFlush] Unable to ForceFlush all metric readers");
    }
  }
  return result;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
