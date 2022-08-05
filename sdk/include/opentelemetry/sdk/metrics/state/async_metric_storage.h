// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class AsyncMetricStorage : public MetricStorage, public AsyncWritableMetricStorage
{
public:
  AsyncMetricStorage(InstrumentDescriptor instrument_descriptor,
                     const AggregationType aggregation_type,
                     const AttributesProcessor *attributes_processor,
                     nostd::shared_ptr<AggregationConfig> aggregation_config,
                     void *state = nullptr)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_type_{aggregation_type},
        attributes_processor_{attributes_processor},
        state_{state},
        cumulative_hash_map_(new AttributesHashMap()),
        delta_hash_map_(new AttributesHashMap()),
        temporal_metric_storage_(instrument_descriptor, aggregation_config)
  {}

  template <class T>
  void Record(const std::unordered_map<MetricAttributes, T, AttributeHashGenerator> &measurements,
              opentelemetry::common::SystemTimestamp observation_time) noexcept
  {
    // process the read measurements - aggregate and store in hashmap
    for (auto &measurement : measurements)
    {
      auto aggr = DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_);
      aggr->Aggregate(measurement.second);
      auto prev = cumulative_hash_map_->Get(measurement.first);
      if (prev)
      {
        auto delta = prev->Diff(*aggr);
        cumulative_hash_map_->Set(measurement.first,
                                  DefaultAggregation::CloneAggregation(
                                      aggregation_type_, instrument_descriptor_, *delta));
        delta_hash_map_->Set(measurement.first, std::move(delta));
      }
      else
      {
        cumulative_hash_map_->Set(
            measurement.first,
            DefaultAggregation::CloneAggregation(aggregation_type_, instrument_descriptor_, *aggr));
        delta_hash_map_->Set(measurement.first, std::move(aggr));
      }
    }
  }

  void RecordLong(
      const std::unordered_map<MetricAttributes, long, AttributeHashGenerator> &measurements,
      opentelemetry::common::SystemTimestamp observation_time) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    Record<long>(measurements, observation_time);
  }

  void RecordDouble(
      const std::unordered_map<MetricAttributes, double, AttributeHashGenerator> &measurements,
      opentelemetry::common::SystemTimestamp observation_time) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
    Record<double>(measurements, observation_time);
  }

  bool Collect(CollectorHandle *collector,
               nostd::span<std::shared_ptr<CollectorHandle>> collectors,
               opentelemetry::common::SystemTimestamp sdk_start_ts,
               opentelemetry::common::SystemTimestamp collection_ts,
               nostd::function_ref<bool(MetricData)> metric_collection_callback) noexcept override
  {

    auto status = temporal_metric_storage_.buildMetrics(collector, collectors, sdk_start_ts,
                                                        collection_ts, std::move(delta_hash_map_),
                                                        metric_collection_callback);
    delta_hash_map_.reset(new AttributesHashMap());
    return status;
  }

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;
  const AttributesProcessor *attributes_processor_;
  void *state_;
  std::unique_ptr<AttributesHashMap> cumulative_hash_map_;
  std::unique_ptr<AttributesHashMap> delta_hash_map_;
  TemporalMetricStorage temporal_metric_storage_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif