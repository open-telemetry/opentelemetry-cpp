// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"

#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#  include "opentelemetry/sdk/metrics/view/view.h"
#  include "opentelemetry/sdk/resource/resource.h"

#  include <list>
#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

struct LastReportedMetrics
{
  std::unique_ptr<AttributesHashMap> attributes_map;
  opentelemetry::common::SystemTimestamp collection_ts;
};

class SyncMetricStorage : public MetricStorage, public WritableMetricStorage
{

public:
  SyncMetricStorage(InstrumentDescriptor instrument_descriptor,
                    const AggregationType aggregation_type,
                    const AttributesProcessor *attributes_processor)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_type_{aggregation_type},
        attributes_hashmap_(new AttributesHashMap()),
        attributes_processor_{attributes_processor}
  {
    create_default_aggregation_ = [&]() -> std::unique_ptr<Aggregation> {
      return std::move(
          DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_));
    };
  }

  void RecordLong(long value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }

    auto attr = attributes_processor_->process(attributes);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }

    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }

    auto attr = attributes_processor_->process(attributes);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
  }

  bool Collect(CollectorHandle *collector,
               nostd::span<std::shared_ptr<CollectorHandle>> collectors,
               opentelemetry::common::SystemTimestamp sdk_start_ts,
               opentelemetry::common::SystemTimestamp collection_ts,
               nostd::function_ref<bool(MetricData &)> callback) noexcept override
  {
    opentelemetry::common::SystemTimestamp last_collection_ts = sdk_start_ts;
    auto aggregation_temporarily = collector->GetAggregationTemporarily();
    // add delta metrics to unreported metrics
    std::shared_ptr<AttributesHashMap> delta_metrics = std::move(attributes_hashmap_);
    for (auto &col : collectors)
    {
      unreported_metrics_[col.get()].push_back(delta_metrics);
    }
    // Get the unreported_metrics_ data for `collector` since last poll
    auto present = unreported_metrics_.find(collector);
    if (present == unreported_metrics_.end())
    {
      // no unreported metrics for the collector, return.
      return true;
    }
    auto unreported_list = std::move(unreported_metrics_[collector]);

    // Iterate over the unreporter metrics for `collector` and do merge
    std::unique_ptr<AttributesHashMap> merged_attributes;
    for (auto &agg_hashmap : unreported_list)
    {
      agg_hashmap->GetAllEnteries(
          [&merged_attributes, this](const MetricAttributes &attributes, Aggregation &aggregation) {
            auto agg = merged_attributes->Get(attributes);
            if (agg)
            {
              merged_attributes->Set(attributes, agg->Merge(aggregation));
            }
            else
            {
              merged_attributes->Set(attributes, create_aggregation());
            }
            return true;
          });
    }

    auto reported = last_reported_metrics_.find(collector);
    if (reported != last_reported_metrics_.end())
    {
      last_collection_ts     = last_reported_metrics_[collector].collection_ts;
      auto last_aggr_hashmap = std::move(last_reported_metrics_[collector].attributes_map);
      if (aggregation_temporarily == AggregationTemporarily::kCummulative)
      {
        // merge current delta to pervious cummulative
        last_aggr_hashmap->GetAllEnteries(
            [&merged_attributes, this](const MetricAttributes &attributes,
                                       Aggregation &aggregation) {
              auto agg = merged_attributes->Get(attributes);
              if (agg)
              {
                merged_attributes->Set(attributes, agg->Merge(aggregation));
              }
              else
              {
                merged_attributes->Set(attributes, create_aggregation());
              }
              return true;
            });
      }
      last_reported_metrics_[collector] = {.attributes_map = std::move(merged_attributes),
                                           .collection_ts  = collection_ts};
    }
    else
    {
      last_reported_metrics_.insert(std::make_pair(
          collector, LastReportedMetrics{.attributes_map = std::move(merged_attributes),
                                         .collection_ts  = collection_ts}));
    }

    AttributesHashMap *result_to_export = (last_reported_metrics_[collector]).attributes_map.get();
    MetricData metric_data;
    metric_data.instrument_descriptor = instrument_descriptor_;
    metric_data.start_ts              = last_collection_ts;
    metric_data.end_ts                = collection_ts;
    result_to_export->GetAllEnteries(
        [&metric_data](const MetricAttributes &attributes, Aggregation &aggregation) {
          PointDataAttributes point_data_attr;
          point_data_attr.point_data = aggregation.ToPoint();
          point_data_attr.attributes = attributes;
          metric_data.point_data_attr_.push_back(point_data_attr);
          return true;
        });
    if (callback(metric_data))
    {
      return true;
    }
    return false;
  }

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
  std::unordered_map<CollectorHandle *, std::list<std::shared_ptr<AttributesHashMap>>>
      unreported_metrics_;
  std::unordered_map<CollectorHandle *, LastReportedMetrics> last_reported_metrics_;
  const AttributesProcessor *attributes_processor_;
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif