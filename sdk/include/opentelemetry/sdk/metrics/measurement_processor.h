// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <map>
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

static std::size_t MakeKey(const MetricReader &metric_reader)
{
  return reinterpret_cast<std::size_t>(&metric_reader);
}

class MeasurementProcessor
{
public:
  virtual void RecordLong(long value) noexcept = 0;

  virtual void RecordLong(long value,
                          const opentelemetry::common::KeyValueIterable &attributes) noexcept = 0;

  virtual void RecordDouble(double value) noexcept = 0;

  virtual void RecordDouble(double value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept = 0;

  virtual bool Collect(MetricReader &reader,
                       AggregationTemporarily aggregation_temporarily,
                       nostd::function_ref<bool(MetricData)> callback) noexcept = 0;
};

class DefaultMeasurementProcessor : public MeasurementProcessor
{
public:
  bool AddMetricStorage(const MetricReader &reader)
  {
    // TBD Check if already present
    // pass intrumentation type, and aggregation type instead of hardcodig below.
    InstrumentDescriptor instr_desc   = {"name", "desc", "1unit", InstrumentType::kCounter,
                                       InstrumentValueType::kLong};
    metric_storages_[MakeKey(reader)] = std::unique_ptr<SyncMetricStorage>(
        new SyncMetricStorage(instr_desc, AggregationType::kSum, new DefaultAttributesProcessor()));
    return true;
  }

  virtual void RecordLong(long value) noexcept override
  {
    for (const auto &kv : metric_storages_)
    {
      kv.second->RecordLong(value);
    }
  }

  virtual void RecordLong(
      long value,
      const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    for (const auto &kv : metric_storages_)
    {
      kv.second->RecordLong(value, attributes);
    }
  }

  virtual void RecordDouble(double value) noexcept override
  {
    for (const auto &kv : metric_storages_)
    {
      kv.second->RecordDouble(value);
    }
  }

  virtual void RecordDouble(
      double value,
      const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    for (const auto &kv : metric_storages_)
    {
      kv.second->RecordDouble(value, attributes);
    }
  }

  bool Collect(MetricReader &reader,
               AggregationTemporarily aggregation_temporarily,
               nostd::function_ref<bool(MetricData)> callback) noexcept override
  {
    auto i = metric_storages_.find(MakeKey(reader));

    // TBD - Remove hardcodings below
    std::vector<MetricCollector *> collectors;
    if (i != metric_storages_.end())
    {

      return i->second->Collect(nullptr, collectors, nullptr, nullptr, callback);
    }
    return false;
  }

private:
  std::map<std::size_t, std::unique_ptr<SyncMetricStorage>> metric_storages_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif