// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk_config.h"

#include <mutex>

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

bool MeterContext::ForEachMeter(
    nostd::function_ref<bool(std::shared_ptr<Meter> &meter)> callback) noexcept
{
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(meter_lock_);
  for (auto &meter : meters_)
  {
    if (!callback(meter))
    {
      return false;
    }
  }
  return true;
}

nostd::span<std::shared_ptr<Meter>> MeterContext::GetMeters() noexcept
{
  // no lock required, as this is called by MeterProvider in thread-safe manner.
  return nostd::span<std::shared_ptr<Meter>>{meters_.data(), meters_.size()};
}

nostd::span<std::shared_ptr<CollectorHandle>> MeterContext::GetCollectors() noexcept
{
  return nostd::span<std::shared_ptr<CollectorHandle>>(collectors_.data(), collectors_.size());
}

opentelemetry::common::SystemTimestamp MeterContext::GetSDKStartTime() noexcept
{
  return sdk_start_ts_;
}

void MeterContext::AddMetricReader(std::shared_ptr<MetricReader> reader) noexcept
{
  auto collector = std::shared_ptr<MetricCollector>{new MetricCollector(this, reader)};
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
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(meter_lock_);
  meters_.push_back(meter);
}

void MeterContext::RemoveMeter(nostd::string_view name,
                               nostd::string_view version,
                               nostd::string_view schema_url)
{
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(meter_lock_);

  std::vector<std::shared_ptr<Meter>> filtered_meters;

  for (auto &meter : meters_)
  {
    auto scope = meter->GetInstrumentationScope();
    if (scope->equal(name, version, schema_url))
    {
      OTEL_INTERNAL_LOG_INFO("[MeterContext::RemoveMeter] removing meter name <"
                             << name << ">, version <" << version << ">, URL <" << schema_url
                             << ">");
    }
    else
    {
      filtered_meters.push_back(meter);
    }
  }

  meters_.swap(filtered_meters);
}

bool MeterContext::Shutdown() noexcept
{
  bool result = true;
  // Shutdown only once.
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
  else
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Shutdown can be invoked only once.");
  }
  return result;
}

bool MeterContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  bool result = true;
  // Simultaneous flush not allowed.
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(forceflush_lock_);
  // Convert to nanos to prevent overflow
  auto timeout_ns = (std::chrono::nanoseconds::max)();
  if (std::chrono::duration_cast<std::chrono::microseconds>(timeout_ns) > timeout)
  {
    timeout_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout);
  }

  auto current_time = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point expire_time;
  auto overflow_checker = (std::chrono::system_clock::time_point::max)();

  // check if the expected expire time doesn't overflow.
  if (overflow_checker - current_time > timeout_ns)
  {
    expire_time =
        current_time + std::chrono::duration_cast<std::chrono::system_clock::duration>(timeout_ns);
  }
  else
  {
    // overflow happens, reset expire time to max.
    expire_time = overflow_checker;
  }

  for (auto &collector : collectors_)
  {
    if (!std::static_pointer_cast<MetricCollector>(collector)->ForceFlush(
            std::chrono::duration_cast<std::chrono::microseconds>(timeout_ns)))
    {
      result = false;
    }

    current_time = std::chrono::system_clock::now();

    if (expire_time >= current_time)
    {
      timeout_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(expire_time - current_time);
    }
    else
    {
      timeout_ns = std::chrono::nanoseconds::zero();
    }
  }
  if (!result)
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::ForceFlush] Unable to ForceFlush all metric readers");
  }
  return result;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
