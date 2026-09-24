// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <mutex>
#include <utility>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#include "opentelemetry/version.h"

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
#  include <stddef.h>
#  include <cstdint>
#  include <functional>
#  include <unordered_map>

#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#endif

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
  std::shared_ptr<AttributesHashMap> delta_metrics;
  {
    std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
    delta_metrics = std::make_shared<AttributesHashMap>(aggregation_config_->cardinality_limit_);
    for (auto it = entries_.begin(); it != entries_.end();)
    {
      auto &entry = *it->second;
      // Bound handles and in-flight ordinary recordings own shared_ptrs.
      // Retain their entry so a write racing with collection is recorded in
      // the next interval instead of becoming detached from the map.
      const bool retained = it->second.use_count() > 1;
      {
        std::lock_guard<std::mutex> entry_guard(entry.lock_);
        if (entry.dirty_)
        {
          delta_metrics->Set(it->first, std::move(entry.current_));
          if (retained)
          {
            entry.current_ = create_default_aggregation_();
          }
          entry.dirty_ = false;
        }
      }
      if (retained)
      {
        ++it;
      }
      else
      {
        // Destroy the entry only after releasing its mutex.
        it = entries_.erase(it);
      }
    }
#else
    delta_metrics = std::move(attributes_hashmap_);
    attributes_hashmap_.reset(new AttributesHashMap(aggregation_config_->cardinality_limit_));
#endif
  }

  return temporal_metric_storage_.buildMetrics(collector, collectors, sdk_start_ts, collection_ts,
                                               delta_metrics, callback);
}

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
std::shared_ptr<BoundSyncWritableMetricStorage> SyncMetricStorage::Bind(
    const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  MetricAttributes filtered{attributes, attributes_processor_.get()};
  return GetOrCreateEntry(filtered);
}

std::shared_ptr<SyncMetricStorage::BoundEntry> SyncMetricStorage::GetOrCreateEntry(
    const MetricAttributes &attributes) noexcept
{
  std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
  auto it = entries_.find(attributes);
  if (it != entries_.end())
  {
    return it->second;
  }

  const auto &overflow           = GetOverflowAttributes();
  auto overflow_it               = entries_.find(overflow);
  const size_t non_overflow_size = entries_.size() - (overflow_it != entries_.end() ? 1 : 0);
  const bool use_overflow =
      attributes == overflow || non_overflow_size >= aggregation_config_->cardinality_limit_;
  if (use_overflow && overflow_it != entries_.end())
  {
    return overflow_it->second;
  }

  auto entry = std::make_shared<BoundEntry>(instrument_descriptor_.value_type_,
                                            create_default_aggregation_());
  return entries_.emplace(use_overflow ? overflow : attributes, std::move(entry)).first->second;
}

void SyncMetricStorage::BoundEntry::RecordLong(int64_t value) noexcept
{
  if (value_type_ != InstrumentValueType::kLong)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[SyncMetricStorage::BoundEntry::RecordLong] Value not recorded - storage value type "
        "is not long");
    return;
  }
  std::lock_guard<std::mutex> guard(lock_);
  current_->Aggregate(value);
  dirty_ = true;
}

void SyncMetricStorage::BoundEntry::RecordDouble(double value) noexcept
{
  if (value_type_ != InstrumentValueType::kDouble)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[SyncMetricStorage::BoundEntry::RecordDouble] Value not recorded - storage value type "
        "is not double");
    return;
  }
  std::lock_guard<std::mutex> guard(lock_);
  current_->Aggregate(value);
  dirty_ = true;
}
#endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
