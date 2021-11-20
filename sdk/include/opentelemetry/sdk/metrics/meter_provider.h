// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include <mutex>
#  include <vector>
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
class MeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  /**
   * Initialize a new meter provider
   * @param exporters The span exporters for this meter provider
   * @param readers The readers for this meter provider.
   * @param views The views for this meter provider
   * @param resource  The resources for this meter provider.
   */
  MeterProvider(std::vector<std::unique_ptr<MeterExporter>> &&exporters,
                std::vector<std::unique_ptr<MeterReader>> &&readers,
                std::vector<std::unique_ptr<View>> &&views,
                sdk::resource::Resource resource = sdk::resource::Resource::Create({})) noexcept;

  /**
   * Initialize a new meter provider with a specified context
   * @param context The shared meter configuration/pipeline for this provider.
   */
  explicit MeterProvider(std::shared_ptr<sdk::metrics::MeterContext> context) noexcept;

  nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version = "",
      nostd::string_view schema_url      = "") noexcept override;

  /**
   * Obtain the resource associated with this meter provider.
   * @return The resource for this meter provider.
   */
  const sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Attaches a meter exporter to list of configured exporters for this Meter provider.
   * @param exporter The meter exporter for this meter provider. This
   * must not be a nullptr.
   *
   * Note: This exporter may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddMeterExporter(std::unique_ptr<MeterExporter> exporter) noexcept;

  /**
   * Attaches a meter reader to list of configured readers for this Meter providers.
   * @param reader The meter reader for this meter provider. This
   * must not be a nullptr.
   *
   * Note: This reader may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddMeterReader(std::unique_ptr<MeterReader> reader) noexcept;

  /**
   * Attaches a View to list of configured Views for this Meter provider.
   * @param view The Views for this meter provider. This
   * must not be a nullptr.
   *
   * Note: This view may not receive any in-flight meter data, but will get newly created meter
   * data. Note: This method is not thread safe, and should ideally be called from main thread.
   */
  void AddView(std::unique_ptr<View> view) noexcept;

  /**
   * Shutdown the meter provider.
   */
  bool Shutdown() noexcept;

  /**
   * Force flush the meter provider.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

private:
  // // order of declaration is important here - meter should destroy only after resource.
  std::vector<std::shared_ptr<sdk::metrics::Meter>> meters_;
  std::shared_ptr<sdk::metrics::MeterContext> context_;
  std::mutex lock_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
