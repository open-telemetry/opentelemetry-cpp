// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"

#  include "opentelemetry/common/macros.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class MetricStorage;
class SyncWritableMetricStorage;
class AsyncWritableMetricsStorge;
class ObservableRegistry;

class Meter final : public opentelemetry::metrics::Meter
{
public:
  /** Construct a new Meter with the given  pipeline. */
  explicit Meter(
      std::shared_ptr<sdk::metrics::MeterContext> meter_context,
      std::unique_ptr<opentelemetry::sdk::instrumentationscope::InstrumentationScope> scope =
          opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create("")) noexcept;

  nostd::shared_ptr<opentelemetry::metrics::Counter<long>> CreateLongCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Counter<double>> CreateDoubleCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> CreateLongObservableCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> CreateDoubleObservableCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<long>> CreateLongHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> CreateLongObservableGauge(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> CreateDoubleObservableGauge(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<long>> CreateLongUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<double>> CreateDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> CreateLongObservableUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>
  CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                      nostd::string_view description = "",
                                      nostd::string_view unit        = "") noexcept override;

  /** Returns the associated instrumentation scope */
  const sdk::instrumentationscope::InstrumentationScope *GetInstrumentationScope() const noexcept;

  OPENTELEMETRY_DEPRECATED_MESSAGE("Please use GetInstrumentationScope instead")
  const sdk::instrumentationscope::InstrumentationScope *GetInstrumentationLibrary() const noexcept
  {
    return GetInstrumentationScope();
  }

  /** collect metrics across all the instruments configured for the meter **/
  std::vector<MetricData> Collect(CollectorHandle *collector,
                                  opentelemetry::common::SystemTimestamp collect_ts) noexcept;

private:
  // order of declaration is important here - instrumentation scope should destroy after
  // meter-context.
  std::unique_ptr<sdk::instrumentationscope::InstrumentationScope> scope_;
  std::shared_ptr<sdk::metrics::MeterContext> meter_context_;
  // Mapping between instrument-name and Aggregation Storage.
  std::unordered_map<std::string, std::shared_ptr<MetricStorage>> storage_registry_;
  std::shared_ptr<ObservableRegistry> observable_registry_;
  std::unique_ptr<SyncWritableMetricStorage> RegisterSyncMetricStorage(
      InstrumentDescriptor &instrument_descriptor);
  std::unique_ptr<AsyncWritableMetricStorage> RegisterAsyncMetricStorage(
      InstrumentDescriptor &instrument_descriptor);
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
