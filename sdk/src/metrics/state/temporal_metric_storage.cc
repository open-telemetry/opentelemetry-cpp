// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

TemporalMetricStorage::TemporalMetricStorage(InstrumentDescriptor instrument_descriptor,
                                             AggregationType aggregation_type,
                                             const AggregationConfig *aggregation_config)
    : instrument_descriptor_(std::move(instrument_descriptor)),
      aggregation_type_(aggregation_type),
      aggregation_config_(aggregation_config),
      instrument_creation_ts_(std::chrono::system_clock::now())
{}

bool TemporalMetricStorage::buildMetrics(CollectorHandle *collector,
                                         nostd::span<std::shared_ptr<CollectorHandle>> collectors,
                                         opentelemetry::common::SystemTimestamp sdk_start_ts,
                                         opentelemetry::common::SystemTimestamp collection_ts,
                                         const std::shared_ptr<AttributesHashMap> &delta_metrics,
                                         nostd::function_ref<bool(MetricData)> callback) noexcept
{
  std::lock_guard<std::mutex> guard(lock_);
  AggregationTemporality aggregation_temporarily =
      collector->GetAggregationTemporality(instrument_descriptor_.type_);
  // Per OTel spec (issue #4062): the start_ts for the first delta collection
  // interval must be the instrument creation time, not the MeterProvider
  // start time (which is what sdk_start_ts carries). For cumulative, sdk_start_ts
  // is acceptable per spec ("creation of the instrument or the timestamp from
  // when the SDK was started, whichever happened first").
  opentelemetry::common::SystemTimestamp last_collection_ts =
      (aggregation_temporarily == AggregationTemporality::kDelta) ? instrument_creation_ts_
                                                                  : sdk_start_ts;

  // Fast path for single collector with delta temporality.
  // This path doesn't need to aggregated-with/contribute-to the unreported_metric_, as there is
  // no other reader configured to collect those data.
  // For synchronous gauge this provides window-scoped last-value semantics: only values recorded
  // since the previous collection are emitted; stale values are not re-exported.
  if (collectors.size() == 1 && aggregation_temporarily == AggregationTemporality::kDelta)
  {
    if (!has_last_delta_collection_ts_)
    {
      last_delta_collection_ts_     = instrument_creation_ts_;
      has_last_delta_collection_ts_ = true;
    }
    // If no metrics, early return
    if (delta_metrics->Size() == 0)
    {
      last_delta_collection_ts_ = collection_ts;
      return true;
    }
    // Create MetricData directly
    MetricData metric_data;
    metric_data.instrument_descriptor   = instrument_descriptor_;
    metric_data.aggregation_temporality = AggregationTemporality::kDelta;
    metric_data.start_ts                = last_delta_collection_ts_;
    metric_data.end_ts                  = collection_ts;
    last_delta_collection_ts_           = collection_ts;

    // Direct conversion of delta metrics to point data
    delta_metrics->GetAllEntries(
        [&metric_data](const MetricAttributes &attributes, Aggregation &aggregation) {
          PointDataAttributes point_data_attr;
          point_data_attr.point_data = aggregation.ToPoint();
          point_data_attr.attributes = attributes;
          metric_data.point_data_attr_.emplace_back(std::move(point_data_attr));
          return true;
        });
    return callback(metric_data);
  }

  const bool async_cumulative =
      InstrumentDescriptorUtil::IsInstrumentTypeAsync(instrument_descriptor_.type_) &&
      aggregation_temporarily == AggregationTemporality::kCumulative;
  bool async_cumulative_overflow_observed =
      async_cumulative && delta_metrics->Has(GetOverflowAttributes());

  if (delta_metrics->Size())
  {
    for (auto &col : collectors)
    {
      unreported_metrics_[col.get()].push_back(delta_metrics);
    }
  }

  // Get the unreported metrics for the `collector` from `unreported metrics stash`
  // since last collection, this will also cleanup the unreported metrics for `collector`
  // from the stash.
  auto present = unreported_metrics_.find(collector);
  if (present == unreported_metrics_.end())
  {
    return true;
  }
  auto unreported_list = std::move(present->second);
  // Iterate over the unreporter metrics for `collector` and store result in `merged_metrics`
  std::unique_ptr<AttributesHashMap> merged_metrics(
      new AttributesHashMap(aggregation_config_ ? aggregation_config_->cardinality_limit_
                                                : kAggregationCardinalityLimit));
  auto merge_aggregation =
      [this, &merged_metrics, async_cumulative, &async_cumulative_overflow_observed](
          const MetricAttributes &attributes, Aggregation &aggregation, bool is_current_delta) {
        auto *existing = merged_metrics->GetOrSetDefault(attributes, [this]() {
          return DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_,
                                                       aggregation_config_);
        });
        if (async_cumulative && is_current_delta &&
            (attributes == GetOverflowAttributes() || !merged_metrics->Has(attributes)))
        {
          async_cumulative_overflow_observed = true;
        }
        auto merged = existing->Merge(aggregation);
        const auto &merged_attributes =
            merged_metrics->Has(attributes) ? attributes : GetOverflowAttributes();
        merged_metrics->Set(merged_attributes, std::move(merged));
        return true;
      };

  auto reported = last_reported_metrics_.find(collector);
  std::unique_ptr<AttributesHashMap> last_aggr_hashmap;
  if (reported != last_reported_metrics_.end())
  {
    last_aggr_hashmap = std::move(reported->second.attributes_map);
    if (aggregation_temporarily == AggregationTemporality::kCumulative)
    {
      // Preserve the existing cumulative series before admitting new ones under the limit.
      last_aggr_hashmap->GetAllEntries(
          [&merge_aggregation](const MetricAttributes &attributes, Aggregation &aggregation) {
            return merge_aggregation(attributes, aggregation, false);
          });
    }
  }

  for (auto &agg_hashmap : unreported_list)
  {
    const bool is_current_delta = agg_hashmap.get() == delta_metrics.get();
    agg_hashmap->GetAllEntries([&merge_aggregation, is_current_delta](
                                   const MetricAttributes &attributes, Aggregation &aggregation) {
      return merge_aggregation(attributes, aggregation, is_current_delta);
    });
  }

  // Cumulative storage admits the previous state first to preserve its cardinality slots.
  // Delta storage carries forward only the unreported metrics.

  if (reported != last_reported_metrics_.end())
  {
    if (aggregation_temporarily != AggregationTemporality::kCumulative)
    {
      last_collection_ts = reported->second.collection_ts;
    }
    last_reported_metrics_[collector] =
        LastReportedMetrics{std::move(merged_metrics), collection_ts};
  }
  else
  {
    last_reported_metrics_.insert(
        std::make_pair(collector, LastReportedMetrics{std::move(merged_metrics), collection_ts}));
  }

  // Generate the MetricData from the final merged_metrics, and invoke callback over it.

  AttributesHashMap *result_to_export = (last_reported_metrics_[collector]).attributes_map.get();
  if (result_to_export->Size() == 0)
  {
    return true;
  }
  MetricData metric_data;
  metric_data.instrument_descriptor   = instrument_descriptor_;
  metric_data.aggregation_temporality = aggregation_temporarily;
  metric_data.start_ts                = last_collection_ts;
  metric_data.end_ts                  = collection_ts;
  // Async observations apply only to the reader whose callback produced delta_metrics.
  result_to_export->GetAllEntries(
      [&metric_data, delta_metrics, async_cumulative, async_cumulative_overflow_observed](
          const MetricAttributes &attributes, Aggregation &aggregation) {
        if (async_cumulative && !delta_metrics->Has(attributes) &&
            !(attributes == GetOverflowAttributes() && async_cumulative_overflow_observed))
        {
          // Omit attribute sets not observed by this reader's callback, while keeping the
          // internal cumulative state.
          return true;
        }

        PointDataAttributes point_data_attr;
        point_data_attr.point_data = aggregation.ToPoint();
        point_data_attr.attributes = attributes;
        metric_data.point_data_attr_.emplace_back(std::move(point_data_attr));
        return true;
      });

  if (metric_data.point_data_attr_.empty())
  {
    return true;
  }
  return callback(metric_data);
}

}  // namespace metrics

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
