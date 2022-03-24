// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/metrics/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/sync_instruments.h"
#  include "opentelemetry/sdk_config.h"

#  include "opentelemetry/version.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace metrics = opentelemetry::metrics;
namespace nostd   = opentelemetry::nostd;

Meter::Meter(std::shared_ptr<MeterContext> meter_context,
             std::unique_ptr<sdk::instrumentationlibrary::InstrumentationLibrary>
                 instrumentation_library) noexcept
    : meter_context_{meter_context}, instrumentation_library_{std::move(instrumentation_library)}
{}

nostd::shared_ptr<metrics::Counter<long>> Meter::CreateLongCounter(nostd::string_view name,
                                                                   nostd::string_view description,
                                                                   nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kCounter, InstrumentValueType::kLong};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::Counter<long>>(
      new LongCounter(instrument_descriptor, std::move(storage)));
}

nostd::shared_ptr<metrics::Counter<double>> Meter::CreateDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::Counter<double>>{
      new DoubleCounter(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<metrics::ObservableCounter<long>> Meter::CreateLongObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableCounter<long>>{
      new LongObservableCounter(name, GetInstrumentationLibrary(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableCounter<double>> Meter::CreateDoubleObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableCounter<double>>{
      new DoubleObservableCounter(name, GetInstrumentationLibrary(), callback, description, unit)};
}

nostd::shared_ptr<metrics::Histogram<long>> Meter::CreateLongHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kHistogram,
      InstrumentValueType::kLong};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::Histogram<long>>{
      new LongHistogram(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<metrics::Histogram<double>> Meter::CreateDoubleHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kHistogram,
      InstrumentValueType::kDouble};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::Histogram<double>>{
      new DoubleHistogram(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<metrics::ObservableGauge<long>> Meter::CreateLongObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableGauge<long>>{
      new LongObservableGauge(name, GetInstrumentationLibrary(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableGauge<double>> Meter::CreateDoubleObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableGauge<double>>{
      new DoubleObservableGauge(name, GetInstrumentationLibrary(), callback, description, unit)};
}

nostd::shared_ptr<metrics::UpDownCounter<long>> Meter::CreateLongUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kUpDownCounter,
      InstrumentValueType::kLong};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::UpDownCounter<long>>{
      new LongUpDownCounter(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<metrics::UpDownCounter<double>> Meter::CreateDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kUpDownCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::UpDownCounter<double>>{
      new DoubleUpDownCounter(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<long>> Meter::CreateLongObservableUpDownCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<long>>{new LongObservableUpDownCounter(
      name, GetInstrumentationLibrary(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>
Meter::CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                           void (*callback)(metrics::ObserverResult<double> &),
                                           nostd::string_view description,
                                           nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>{
      new DoubleObservableUpDownCounter(name, GetInstrumentationLibrary(), callback, description,
                                        unit)};
}

const sdk::instrumentationlibrary::InstrumentationLibrary *Meter::GetInstrumentationLibrary()
    const noexcept
{
  return instrumentation_library_.get();
}

std::unique_ptr<WritableMetricStorage> Meter::RegisterMetricStorage(
    InstrumentDescriptor &instrument_descriptor)
{
  auto view_registry = meter_context_->GetViewRegistry();
  std::unique_ptr<WritableMetricStorage> storages(new MultiMetricStorage());

  auto success = view_registry->FindViews(
      instrument_descriptor, *instrumentation_library_,
      [this, &instrument_descriptor, &storages](const View &view) {
        auto view_instr_desc         = instrument_descriptor;
        view_instr_desc.name_        = view.GetName();
        view_instr_desc.description_ = view.GetDescription();
        auto storage                 = std::shared_ptr<SyncMetricStorage>(new SyncMetricStorage(
            view_instr_desc, view.GetAggregationType(), &view.GetAttributesProcessor(),
            NoExemplarReservoir::GetNoExemplarReservoir()));
        storage_registry_[instrument_descriptor.name_] = storage;
        auto multi_storage = static_cast<MultiMetricStorage *>(storages.get());
        multi_storage->AddStorage(storage);
        return true;
      });

  if (!success)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[Meter::RegisterMetricStorage] - Error during finding matching views."
        << "Some of the matching view configurations mayn't be used for metric collection");
  }
  return std::move(storages);
}

/** collect metrics across all the meters **/
bool Meter::collect(CollectorHandle *collector,
                    opentelemetry::common::SystemTimestamp collect_ts,
                    nostd::function_ref<bool(MetricData &)> callback) noexcept
{
  std::vector<MetricData> data;
  for (auto &metric_storage : storage_registry_)
  {
    // TBD - this needs to be asynchronous
    metric_storage.second->Collect(collector, meter_context_->GetCollectors(),
                                   meter_context_->GetSDKStartTime(), collect_ts,
                                   [&callback](MetricData &metric_data) {
                                     callback(metric_data);
                                     return true;
                                   });
  }
  return true;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
