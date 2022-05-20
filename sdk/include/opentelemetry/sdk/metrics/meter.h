// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"

#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class MetricStorage;
class WritableMetricStorage;

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

  void CreateLongObservableCounter(nostd::string_view name,
                                   void (*callback)(opentelemetry::metrics::ObserverResult<long> &,
                                                    void *),
                                   nostd::string_view description = "",
                                   nostd::string_view unit        = "",
                                   void *state                    = nullptr) noexcept override;

  void CreateDoubleObservableCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<double> &, void *),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      void *state                    = nullptr) noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<long>> CreateLongHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  void CreateLongObservableGauge(nostd::string_view name,
                                 void (*callback)(opentelemetry::metrics::ObserverResult<long> &,
                                                  void *),
                                 nostd::string_view description = "",
                                 nostd::string_view unit        = "",
                                 void *state                    = nullptr) noexcept override;

  void CreateDoubleObservableGauge(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<double> &, void *),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      void *state                    = nullptr) noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<long>> CreateLongUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<double>> CreateDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override;

  void CreateLongObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<long> &, void *),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      void *state                    = nullptr) noexcept override;

  void CreateDoubleObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(opentelemetry::metrics::ObserverResult<double> &, void *),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      void *state                    = nullptr) noexcept override;

  /** Returns the associated instruementation library */
  const sdk::instrumentationlibrary::InstrumentationLibrary *GetInstrumentationLibrary()
      const noexcept;

  /** collect metrics across all the instruments configured for the meter **/
  std::vector<MetricData> Collect(CollectorHandle *collector,
                                  opentelemetry::common::SystemTimestamp collect_ts) noexcept;

private:
  // order of declaration is important here - instrumentation library should destroy after
  // meter-context.
  std::unique_ptr<sdk::instrumentationlibrary::InstrumentationLibrary> instrumentation_library_;
  std::shared_ptr<sdk::metrics::MeterContext> meter_context_;
  // Mapping between instrument-name and Aggregation Storage.
  std::unordered_map<std::string, std::shared_ptr<MetricStorage>> storage_registry_;

  std::unique_ptr<WritableMetricStorage> RegisterMetricStorage(
      InstrumentDescriptor &instrument_descriptor);

  template <class T>
  void RegisterAsyncMetricStorage(InstrumentDescriptor &instrument_descriptor,
                                  void (*callback)(opentelemetry::metrics::ObserverResult<T> &,
                                                   void *),
                                  void *state = nullptr)
  {
    auto view_registry = meter_context_->GetViewRegistry();
    auto success       = view_registry->FindViews(
        instrument_descriptor, *instrumentation_library_,
        [this, &instrument_descriptor, callback, state](const View &view) {
          auto view_instr_desc = instrument_descriptor;
          if (!view.GetName().empty())
          {
            view_instr_desc.name_ = view.GetName();
          }
          if (!view.GetDescription().empty())
          {
            view_instr_desc.description_ = view.GetDescription();
          }
          auto storage = std::shared_ptr<AsyncMetricStorage<T>>(
              new AsyncMetricStorage<T>(view_instr_desc, view.GetAggregationType(), callback,
                                        &view.GetAttributesProcessor(), state));
          storage_registry_[instrument_descriptor.name_] = storage;
          return true;
        });
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
