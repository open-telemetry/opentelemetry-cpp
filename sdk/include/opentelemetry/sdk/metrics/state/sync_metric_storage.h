// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/metric_storage.h"

#include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#include <list>
#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class SyncMetricStorage : public MetricStorage, public SyncWritableMetricStorage
{

public:
  SyncMetricStorage(InstrumentDescriptor instrument_descriptor,
                    const AggregationType aggregation_type,
                    const AttributesProcessor *attributes_processor,
                    nostd::shared_ptr<ExemplarReservoir> &&exemplar_reservoir
                        OPENTELEMETRY_MAYBE_UNUSED,
                    const AggregationConfig *aggregation_config)
      : instrument_descriptor_(instrument_descriptor),
        attributes_hashmap_(new AttributesHashMap()),
        attributes_processor_(attributes_processor),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
        exemplar_reservoir_(exemplar_reservoir),
#endif
        temporal_metric_storage_(instrument_descriptor, aggregation_type, aggregation_config)
  {
    create_default_aggregation_ = [&, aggregation_type,
                                   aggregation_config]() -> std::unique_ptr<Aggregation> {
      return DefaultAggregation::CreateAggregation(aggregation_type, instrument_descriptor_,
                                                   aggregation_config);
    };
  }

  void RecordLong(int64_t value,
                  const opentelemetry::context::Context &context
                      OPENTELEMETRY_MAYBE_UNUSED) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    exemplar_reservoir_->OfferMeasurement(value, {}, context, std::chrono::system_clock::now());
#endif
    static size_t hash = opentelemetry::sdk::common::GetHash("");
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(create_default_aggregation_, hash)->Aggregate(value);
  }

  void RecordLong(int64_t value,
                  const opentelemetry::common::KeyValueIterable &attributes,
                  const opentelemetry::context::Context &context
                      OPENTELEMETRY_MAYBE_UNUSED) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
#endif
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes, [this](nostd::string_view key) {
          if (attributes_processor_)
          {
            return attributes_processor_->isPresent(key);
          }
          else
          {
            return true;
          }
        });

    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(attributes, create_default_aggregation_, hash)
        ->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::context::Context &context
                        OPENTELEMETRY_MAYBE_UNUSED) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    exemplar_reservoir_->OfferMeasurement(value, {}, context, std::chrono::system_clock::now());
#endif
    static size_t hash = opentelemetry::sdk::common::GetHash("");
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(create_default_aggregation_, hash)->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes,
                    const opentelemetry::context::Context &context
                        OPENTELEMETRY_MAYBE_UNUSED) noexcept override
  {
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
#endif
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
#endif
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes, [this](nostd::string_view key) {
          if (attributes_processor_)
          {
            return attributes_processor_->isPresent(key);
          }
          else
          {
            return true;
          }
        });
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(attributes, create_default_aggregation_, hash)
        ->Aggregate(value);
  }

  bool Collect(CollectorHandle *collector,
               nostd::span<std::shared_ptr<CollectorHandle>> collectors,
               opentelemetry::common::SystemTimestamp sdk_start_ts,
               opentelemetry::common::SystemTimestamp collection_ts,
               nostd::function_ref<bool(MetricData)> callback) noexcept override;

private:
  InstrumentDescriptor instrument_descriptor_;
  // hashmap to maintain the metrics for delta collection (i.e, collection since last Collect call)
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
  // unreported metrics stash for all the collectors
  std::unordered_map<CollectorHandle *, std::list<std::shared_ptr<AttributesHashMap>>>
      unreported_metrics_;
  // last reported metrics stash for all the collectors.
  std::unordered_map<CollectorHandle *, LastReportedMetrics> last_reported_metrics_;
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
  const AttributesProcessor *attributes_processor_;
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
  nostd::shared_ptr<ExemplarReservoir> exemplar_reservoir_;
#endif
  TemporalMetricStorage temporal_metric_storage_;
  opentelemetry::common::SpinLockMutex attribute_hashmap_lock_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
