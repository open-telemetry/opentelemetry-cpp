// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#  include "opentelemetry/sdk/metrics/view/meter_selector.h"
#  include "opentelemetry/sdk/metrics/view/view_registry.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

#  include <chrono>
#  include <memory>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// forward declaration
class Meter;
class MetricReader;

/**
 * A class which stores the MeterProvider context.

 */
class MeterContext : public std::enable_shared_from_this<MeterContext>
{
public:
  /**
   * Initialize a new meter provider
   * @param readers The readers to be configured with meter context.
   * @param views The views to be configured with meter context.
   * @param resource  The resource for this meter context.
   */
  MeterContext(
      std::unique_ptr<ViewRegistry> views = std::unique_ptr<ViewRegistry>(new ViewRegistry()),
      opentelemetry::sdk::resource::Resource resource =
          opentelemetry::sdk::resource::Resource::Create({})) noexcept;

  /**
   * Obtain the resource associated with this meter context.
   * @return The resource for this meter context
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Obtain the View Registry configured
   * @return The reference to view registry
   */
  ViewRegistry *GetViewRegistry() const noexcept;

  /**
   * Obtain the  configured meters.
   *
   */
  nostd::span<std::shared_ptr<Meter>> GetMeters() noexcept;

  /**
   * Obtain the configured collectors.
   *
   */
  nostd::span<std::shared_ptr<CollectorHandle>> GetCollectors() noexcept;

  /**
   * GET SDK Start time
   *
   */
  opentelemetry::common::SystemTimestamp GetSDKStartTime() noexcept;

  /**
   * Attaches a metric reader to list of configured readers for this Meter context.
   * @param reader The metric reader for this meter context. This
   * must not be a nullptr.
   *
   * Note: This reader may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddMetricReader(std::unique_ptr<MetricReader> reader) noexcept;

  /**
   * Attaches a View to list of configured Views for this Meter context.
   * @param view The Views for this meter context. This
   * must not be a nullptr.
   *
   * Note: This view may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddView(std::unique_ptr<InstrumentSelector> instrument_selector,
               std::unique_ptr<MeterSelector> meter_selector,
               std::unique_ptr<View> view) noexcept;

  /**
   * Adds a meter to the list of configured meters.
   * Note: This method is INTERNAL to sdk not thread safe.
   *
   * @param meter
   */
  void AddMeter(std::shared_ptr<Meter> meter);

  /**
   * Force all active Collectors to flush any buffered meter data
   * within the given timeout.
   */

  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Shutdown the Collectors associated with this meter provider.
   */
  bool Shutdown() noexcept;

private:
  opentelemetry::sdk::resource::Resource resource_;
  std::vector<std::shared_ptr<CollectorHandle>> collectors_;
  std::unique_ptr<ViewRegistry> views_;
  opentelemetry::common::SystemTimestamp sdk_start_ts_;
  std::vector<std::shared_ptr<Meter>> meters_;

  std::atomic_flag shutdown_latch_ = ATOMIC_FLAG_INIT;
  opentelemetry::common::SpinLockMutex forceflush_lock_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
