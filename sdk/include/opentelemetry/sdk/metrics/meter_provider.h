// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include <mutex>
#  include <vector>
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/metrics/meter_provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// forward declaration
class MetricCollector;
class MetricReader;

class MeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  /**
   * Initialize a new meter provider
   * @param views The views for this meter provider
   * @param resource  The resources for this meter provider.
   */
  MeterProvider(
      std::unique_ptr<ViewRegistry> views = std::unique_ptr<ViewRegistry>(new ViewRegistry()),
      sdk::resource::Resource resource    = sdk::resource::Resource::Create({})) noexcept;

  /**
   * Initialize a new meter provider with a specified context
   * @param context The shared meter configuration/pipeline for this provider.
   */
  explicit MeterProvider(std::shared_ptr<sdk::metrics::MeterContext> context) noexcept;

  nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view name,
      nostd::string_view version    = "",
      nostd::string_view schema_url = "") noexcept override;

  /**
   * Obtain the resource associated with this meter provider.
   * @return The resource for this meter provider.
   */
  const sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Attaches a metric reader to list of configured readers for this Meter providers.
   * @param reader The metric reader for this meter provider. This
   * must not be a nullptr.
   *
   * Note: This reader may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddMetricReader(std::unique_ptr<MetricReader> reader) noexcept;

  /**
   * Attaches a View to list of configured Views for this Meter provider.
   * @param view The Views for this meter provider. This
   * must not be a nullptr.
   *
   * Note: This view may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddView(std::unique_ptr<InstrumentSelector> instrument_selector,
               std::unique_ptr<MeterSelector> meter_selector,
               std::unique_ptr<View> view) noexcept;

  /**
   * Shutdown the meter provider.
   */
  bool Shutdown() noexcept;

  /**
   * Force flush the meter provider.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

private:
  std::shared_ptr<sdk::metrics::MeterContext> context_;
  std::mutex lock_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
