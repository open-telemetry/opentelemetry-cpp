// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter.h"
#include <cstdint>
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/async_instruments.h"
#include "opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#include "opentelemetry/sdk/metrics/state/observable_registry.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/sync_instruments.h"
#include "opentelemetry/sdk_config.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace metrics = opentelemetry::metrics;
namespace nostd   = opentelemetry::nostd;

Meter::Meter(
    std::weak_ptr<MeterContext> meter_context,
    std::unique_ptr<sdk::instrumentationscope::InstrumentationScope> instrumentation_scope) noexcept
    : scope_{std::move(instrumentation_scope)},
      meter_context_{meter_context},
      observable_registry_(new ObservableRegistry())
{}

nostd::unique_ptr<metrics::Counter<uint64_t>> Meter::CreateUInt64Counter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateUInt64Counter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::Counter<uint64_t>>(
        new metrics::NoopCounter<uint64_t>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kCounter, InstrumentValueType::kLong};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::Counter<uint64_t>>(
      new LongCounter<uint64_t>(instrument_descriptor, std::move(storage)));
}

nostd::unique_ptr<metrics::Counter<double>> Meter::CreateDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateDoubleCounter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::Counter<double>>(
        new metrics::NoopCounter<double>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::Counter<double>>{
      new DoubleCounter(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> Meter::CreateInt64ObservableCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateInt64ObservableCounter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableCounter,
      InstrumentValueType::kLong};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>
Meter::CreateDoubleObservableCounter(nostd::string_view name,
                                     nostd::string_view description,
                                     nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateDoubleObservableCounter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

nostd::unique_ptr<metrics::Histogram<uint64_t>> Meter::CreateUInt64Histogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateUInt64Histogram - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::Histogram<uint64_t>>(
        new metrics::NoopHistogram<uint64_t>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kHistogram,
      InstrumentValueType::kLong};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::Histogram<uint64_t>>{
      new LongHistogram<uint64_t>(instrument_descriptor, std::move(storage))};
}

nostd::unique_ptr<metrics::Histogram<double>> Meter::CreateDoubleHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateDoubleHistogram - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::Histogram<double>>(
        new metrics::NoopHistogram<double>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kHistogram,
      InstrumentValueType::kDouble};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::Histogram<double>>{
      new DoubleHistogram(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> Meter::CreateInt64ObservableGauge(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateInt64ObservableGauge - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableGauge,
      InstrumentValueType::kLong};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> Meter::CreateDoubleObservableGauge(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateDoubleObservableGauge - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableGauge,
      InstrumentValueType::kDouble};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

nostd::unique_ptr<metrics::UpDownCounter<int64_t>> Meter::CreateInt64UpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateInt64UpDownCounter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::UpDownCounter<int64_t>>(
        new metrics::NoopUpDownCounter<int64_t>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kUpDownCounter,
      InstrumentValueType::kLong};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::UpDownCounter<int64_t>>{
      new LongUpDownCounter(instrument_descriptor, std::move(storage))};
}

nostd::unique_ptr<metrics::UpDownCounter<double>> Meter::CreateDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR("Meter::CreateDoubleUpDownCounter - failed. Invalid parameters."
                            << name << " " << description << " " << unit
                            << ". Measurements won't be recorded.");
    return nostd::unique_ptr<metrics::UpDownCounter<double>>(
        new metrics::NoopUpDownCounter<double>(name, description, unit));
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kUpDownCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterSyncMetricStorage(instrument_descriptor);
  return nostd::unique_ptr<metrics::UpDownCounter<double>>{
      new DoubleUpDownCounter(instrument_descriptor, std::move(storage))};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>
Meter::CreateInt64ObservableUpDownCounter(nostd::string_view name,
                                          nostd::string_view description,
                                          nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR(
        "Meter::CreateInt64ObservableUpDownCounter - failed. Invalid parameters -"
        << name << " " << description << " " << unit << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableUpDownCounter,
      InstrumentValueType::kLong};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>
Meter::CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                           nostd::string_view description,
                                           nostd::string_view unit) noexcept
{
  if (!ValidateInstrument(name, description, unit))
  {
    OTEL_INTERNAL_LOG_ERROR(
        "Meter::CreateDoubleObservableUpDownCounter - failed. Invalid parameters."
        << name << " " << description << " " << unit << ". Measurements won't be recorded.");
    return GetNoopObservableInsrument();
  }
  InstrumentDescriptor instrument_descriptor = {
      std::string{name.data(), name.size()}, std::string{description.data(), description.size()},
      std::string{unit.data(), unit.size()}, InstrumentType::kObservableUpDownCounter,
      InstrumentValueType::kDouble};
  auto storage = RegisterAsyncMetricStorage(instrument_descriptor);
  return nostd::shared_ptr<metrics::ObservableInstrument>{
      new ObservableInstrument(instrument_descriptor, std::move(storage), observable_registry_)};
}

const sdk::instrumentationscope::InstrumentationScope *Meter::GetInstrumentationScope()
    const noexcept
{
  return scope_.get();
}

std::unique_ptr<SyncWritableMetricStorage> Meter::RegisterSyncMetricStorage(
    InstrumentDescriptor &instrument_descriptor)
{
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(storage_lock_);
  auto ctx = meter_context_.lock();
  if (!ctx)
  {
    OTEL_INTERNAL_LOG_ERROR("[Meter::RegisterMetricStorage] - Error during finding matching views."
                            << "The metric context is invalid");
    return nullptr;
  }
  auto view_registry = ctx->GetViewRegistry();
  std::unique_ptr<SyncWritableMetricStorage> storages(new SyncMultiMetricStorage());

  auto success = view_registry->FindViews(
      instrument_descriptor, *scope_, [this, &instrument_descriptor, &storages](const View &view) {
        auto view_instr_desc = instrument_descriptor;
        if (!view.GetName().empty())
        {
          view_instr_desc.name_ = view.GetName();
        }
        if (!view.GetDescription().empty())
        {
          view_instr_desc.description_ = view.GetDescription();
        }
        auto multi_storage = static_cast<SyncMultiMetricStorage *>(storages.get());

        auto storage = std::shared_ptr<SyncMetricStorage>(new SyncMetricStorage(
            view_instr_desc, view.GetAggregationType(), &view.GetAttributesProcessor(),
            ExemplarReservoir::GetNoExemplarReservoir(), view.GetAggregationConfig()));
        storage_registry_[instrument_descriptor.name_] = storage;
        multi_storage->AddStorage(storage);
        return true;
      });

  if (!success)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[Meter::RegisterMetricStorage] - Error during finding matching views."
        << "Some of the matching view configurations mayn't be used for metric collection");
  }
  return storages;
}

std::unique_ptr<AsyncWritableMetricStorage> Meter::RegisterAsyncMetricStorage(
    InstrumentDescriptor &instrument_descriptor)
{
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(storage_lock_);
  auto ctx = meter_context_.lock();
  if (!ctx)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[Meter::RegisterAsyncMetricStorage] - Error during finding matching views."
        << "The metric context is invalid");
    return nullptr;
  }
  auto view_registry = ctx->GetViewRegistry();
  std::unique_ptr<AsyncWritableMetricStorage> storages(new AsyncMultiMetricStorage());
  auto success = view_registry->FindViews(
      instrument_descriptor, *GetInstrumentationScope(),
      [this, &instrument_descriptor, &storages](const View &view) {
        auto view_instr_desc = instrument_descriptor;
        if (!view.GetName().empty())
        {
          view_instr_desc.name_ = view.GetName();
        }
        if (!view.GetDescription().empty())
        {
          view_instr_desc.description_ = view.GetDescription();
        }
        auto storage = std::shared_ptr<AsyncMetricStorage>(
            new AsyncMetricStorage(view_instr_desc, view.GetAggregationType(),
                                   &view.GetAttributesProcessor(), view.GetAggregationConfig()));
        storage_registry_[instrument_descriptor.name_] = storage;
        static_cast<AsyncMultiMetricStorage *>(storages.get())->AddStorage(storage);
        return true;
      });
  if (!success)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[Meter::RegisterAsyncMetricStorage] - Error during finding matching views."
        << "Some of the matching view configurations mayn't be used for metric collection");
  }
  return storages;
}

/** collect metrics across all the meters **/
std::vector<MetricData> Meter::Collect(CollectorHandle *collector,
                                       opentelemetry::common::SystemTimestamp collect_ts) noexcept
{
  observable_registry_->Observe(collect_ts);
  std::vector<MetricData> metric_data_list;
  auto ctx = meter_context_.lock();
  if (!ctx)
  {
    OTEL_INTERNAL_LOG_ERROR("[Meter::Collect] - Error during collection."
                            << "The metric context is invalid");
    return std::vector<MetricData>{};
  }
  std::lock_guard<opentelemetry::common::SpinLockMutex> guard(storage_lock_);
  for (auto &metric_storage : storage_registry_)
  {
    metric_storage.second->Collect(collector, ctx->GetCollectors(), ctx->GetSDKStartTime(),
                                   collect_ts, [&metric_data_list](MetricData metric_data) {
                                     metric_data_list.push_back(metric_data);
                                     return true;
                                   });
  }
  return metric_data_list;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
