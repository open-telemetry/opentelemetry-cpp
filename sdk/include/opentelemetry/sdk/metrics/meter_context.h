// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include <memory>
#  include <vector>
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/view.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * A class which stores the MeterProvider context.

 */
class MeterContext
{
public:
  /**
   * Initialize a new meter provider
   * @param exporters The exporters to be configured with meter context
   * @param readers The readers to be configured with meter context.
   * @param views The views to be configured with meter context.
   * @param resource  The resource for this meter context.
   */
  MeterContext(std::vector<std::unique_ptr<sdk::metrics::MetricExporter>> &&exporters,
               std::vector<std::unique_ptr<MetricReader>> &&readers,
               std::vector<std::unique_ptr<View>> &&views,
               opentelemetry::sdk::resource::Resource resource =
                   opentelemetry::sdk::resource::Resource::Create({})) noexcept;

  /**
   * Obtain the resource associated with this meter context.
   * @return The resource for this meter context
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Attaches a metric exporter to list of configured exporters for this Meter context.
   * @param exporter The metric exporter for this meter context. This
   * must not be a nullptr.
   *
   * Note: This exporter may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddMetricExporter(std::unique_ptr<MetricExporter> exporter) noexcept;

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
  void AddView(std::unique_ptr<View> view) noexcept;

  /**
   * Force all active Exporters and Readers to flush any buffered meter data
   * within the given timeout.
   */

  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Shutdown the Exporters and Readers associated with this meter provider.
   */
  bool Shutdown() noexcept;

private:
  opentelemetry::sdk::resource::Resource resource_;
  std::vector<std::unique_ptr<MetricExporter>> exporters_;
  std::vector<std::unique_ptr<MetricReader>> readers_;
  std::vector<std::unique_ptr<View>> views_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
