// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/measurement_processor.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Meter final : public opentelemetry::metrics::Meter
{
public:
  /** Construct a new Meter with the given  pipeline. */
  explicit Meter(std::shared_ptr<sdk::metrics::MeterContext> meter_context,
                 std::unique_ptr<opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary>
                     instrumentation_library =
                         opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create(
                             "")) noexcept;

  nostd::shared_ptr<opentelemetry::metrics::Counter<long>> CreateLongCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Counter<double>> CreateDoubleCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableCounter<long>> CreateLongObservableCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableCounter<double>>
  CreateDoubleObservableCounter(nostd::string_view name,
                                void (*callback)(opentelemetry::metrics::ObserverResult<double> &),
                                nostd::string_view description = "",
                                nostd::string_view unit        = "1") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<long>> CreateLongHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableGauge<long>> CreateLongObservableGauge(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableGauge<double>> CreateDoubleObservableGauge(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<double> &),
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

  nostd::shared_ptr<opentelemetry::metrics::ObservableUpDownCounter<long>>
  CreateLongObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::ObservableUpDownCounter<double>>
  CreateDoubleObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  /** Returns the associated instruementation library */
  const sdk::instrumentationlibrary::InstrumentationLibrary *GetInstrumentationLibrary()
      const noexcept;

  /** Returns the associated measurement processor */
  MeasurementProcessor *GetMeasurementProcessor() const noexcept;

private:
  // order of declaration is important here - instrumentation library should destroy after
  // meter-context.
  std::unique_ptr<sdk::instrumentationlibrary::InstrumentationLibrary> instrumentation_library_;
  std::shared_ptr<sdk::metrics::MeterContext> meter_context_;
  // Mapping between instrument-name and Aggregation Storage.
  std::unordered_map<std::string, std::shared_ptr<MetricStorage>> storage_registry_;

  std::unique_ptr<WritableMetricStorage> RegisterMetricStorage(
      InstrumentDescriptor &instrument_descriptor);
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
