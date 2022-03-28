// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
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
                    const AttributesProcessor *attributes_processor,
                    nostd::shared_ptr<ExemplarReservoir> &&exemplar_reservoir)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_type_{aggregation_type},
        attributes_hashmap_(new AttributesHashMap()),
        attributes_processor_{attributes_processor},
        exemplar_reservoir_(exemplar_reservoir)
  {
    create_default_aggregation_ = [&]() -> std::unique_ptr<Aggregation> {
      return std::move(
          DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_));
    };
  }

  void RecordLong(long value, const opentelemetry::context::Context &context) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    exemplar_reservoir_->OfferMeasurement(value, {}, context, std::chrono::system_clock::now());
    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes,
                  const opentelemetry::context::Context &context) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }

    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
    auto attr = attributes_processor_->process(attributes);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value, const opentelemetry::context::Context &context) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
    exemplar_reservoir_->OfferMeasurement(value, {}, context, std::chrono::system_clock::now());
    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes,
                    const opentelemetry::context::Context &context) noexcept override
  {
    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
    exemplar_reservoir_->OfferMeasurement(value, attributes, context,
                                          std::chrono::system_clock::now());
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

    // Add the current delta metrics to `unreported metrics stash` for all the collectors,
    // this will also empty the delta metrics hashmap, and make it available for
    // recordings
    std::shared_ptr<AttributesHashMap> delta_metrics = std::move(attributes_hashmap_);
    attributes_hashmap_.reset(new AttributesHashMap);
    for (auto &col : collectors)
    {
      unreported_metrics_[col.get()].push_back(delta_metrics);
    }

    // Get the unreported metrics for the `collector` from `unreported metrics stash`
    // since last collection, this will also cleanup the unreported metrics for `collector`
    // from the stash.
    auto present = unreported_metrics_.find(collector);
    if (present == unreported_metrics_.end())
    {
      // no unreported metrics for the collector, return.
      return true;
    }
    auto unreported_list = std::move(unreported_metrics_[collector]);

    // Iterate over the unreporter metrics for `collector` and store result in `merged_metrics`
    std::unique_ptr<AttributesHashMap> merged_metrics(new AttributesHashMap);
    for (auto &agg_hashmap : unreported_list)
    {
      agg_hashmap->GetAllEnteries([&merged_metrics, this](const MetricAttributes &attributes,
                                                          Aggregation &aggregation) {
        auto agg = merged_metrics->Get(attributes);
        if (agg)
        {
          merged_metrics->Set(attributes, std::move(agg->Merge(aggregation)));
        }
        else
        {
          merged_metrics->Set(
              attributes, std::move(DefaultAggregation::CreateAggregation(instrument_descriptor_)
                                        ->Merge(aggregation)));
          merged_metrics->GetAllEnteries(
              [](const MetricAttributes &attr, Aggregation &aggr) { return true; });
        }
        return true;
      });
    }
    // Get the last reported metrics for the `collector` from `last reported metrics` stash
    //   - If the aggregation_temporarily for the collector is cumulative
    //       - Merge the last reported metrics with unreported metrics (which is in merged_metrics),
    //           Final result of merge would be in merged_metrics.
    //       - Move the final merge to the `last reported metrics` stash.
    //   - If the aggregation_temporarily is delta
    //       - Store the unreported metrics for `collector`(which is in merged_mtrics) to
    //          `last reported metrics` stash.

    auto reported = last_reported_metrics_.find(collector);
    if (reported != last_reported_metrics_.end())
    {
      last_collection_ts     = last_reported_metrics_[collector].collection_ts;
      auto last_aggr_hashmap = std::move(last_reported_metrics_[collector].attributes_map);
      if (aggregation_temporarily == AggregationTemporarily::kCumulative)
      {
        // merge current delta to previous cumulative
        last_aggr_hashmap->GetAllEnteries(
            [&merged_metrics, this](const MetricAttributes &attributes, Aggregation &aggregation) {
              auto agg = merged_metrics->Get(attributes);
              if (agg)
              {
                merged_metrics->Set(attributes, agg->Merge(aggregation));
              }
              else
              {
                merged_metrics->Set(attributes,
                                    DefaultAggregation::CreateAggregation(instrument_descriptor_));
              }
              return true;
            });
      }
      last_reported_metrics_[collector] =
          LastReportedMetrics{std::move(merged_metrics), collection_ts};
    }
    else
    {
      merged_metrics->GetAllEnteries(
          [](const MetricAttributes &attr, Aggregation &aggr) { return true; });
      last_reported_metrics_.insert(
          std::make_pair(collector, LastReportedMetrics{std::move(merged_metrics), collection_ts}));
    }

    // Generate the MetricData from the final merged_metrics, and invoke callback over it.

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

  // hashmap to maintain the metrics for delta collection (i.e, collection since last Collect call)
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
  // unreported metrics stash for all the collectors
  std::unordered_map<CollectorHandle *, std::list<std::shared_ptr<AttributesHashMap>>>
      unreported_metrics_;
  // last reported metrics stash for all the collectors.
  std::unordered_map<CollectorHandle *, LastReportedMetrics> last_reported_metrics_;
  const AttributesProcessor *attributes_processor_;
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
  nostd::shared_ptr<ExemplarReservoir> exemplar_reservoir_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif