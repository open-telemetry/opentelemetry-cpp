// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

bool SyncMetricStorage::Collect(CollectorHandle *collector,
                                nostd::span<std::shared_ptr<CollectorHandle>> collectors,
                                opentelemetry::common::SystemTimestamp sdk_start_ts,
                                opentelemetry::common::SystemTimestamp collection_ts,
                                nostd::function_ref<bool(MetricData)> callback) noexcept
{
  opentelemetry::common::SystemTimestamp last_collection_ts = sdk_start_ts;
  auto aggregation_temporarily = collector->GetAggregationTemporality();

  // Add the current delta metrics to `unreported metrics stash` for all the collectors,
  // this will also empty the delta metrics hashmap, and make it available for
  // recordings
  std::shared_ptr<AttributesHashMap> delta_metrics = std::move(attributes_hashmap_);
  attributes_hashmap_.reset(new AttributesHashMap);

  return temporal_metric_storage_.buildMetrics(collector, collectors, sdk_start_ts, collection_ts,
                                               std::move(delta_metrics), callback);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
