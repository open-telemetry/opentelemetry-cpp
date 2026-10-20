// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/metric_storage.h"
#include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/version.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_predicate.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class SyncMetricStorage : public MetricStorage, public SyncWritableMetricStorage
{

public:
  SyncMetricStorage(const InstrumentDescriptor &instrument_descriptor,
                    const AggregationType aggregation_type,
                    std::shared_ptr<const AttributesProcessor> attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
                    ExemplarFilterType exemplar_filter_type,
                    nostd::shared_ptr<ExemplarReservoir> &&exemplar_reservoir,
#endif
                    const AggregationConfig *aggregation_config)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_config_(AggregationConfig::GetOrDefault(aggregation_config)),
#ifndef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
        attributes_hashmap_(
            std::make_unique<AttributesHashMap>(aggregation_config_->cardinality_limit_)),
#endif
        attributes_processor_(std::move(attributes_processor)),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
        exemplar_filter_type_(exemplar_filter_type),
        exemplar_reservoir_(std::move(exemplar_reservoir)),
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
    if (ExemplarFilterEnabled(exemplar_filter_type_, context))
    {
      exemplar_reservoir_->OfferMeasurement(value, {}, context);
    }
#endif
    static MetricAttributes attr = MetricAttributes{};
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
    GetOrCreateEntry(attr)->RecordLong(value);
#else
    std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
#endif
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
    if (ExemplarFilterEnabled(exemplar_filter_type_, context))
    {
      exemplar_reservoir_->OfferMeasurement(value, attributes, context);
    }
#endif

    MetricAttributes attr{attributes, attributes_processor_.get()};
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
    GetOrCreateEntry(attr)->RecordLong(value);
#else
    std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
    // cppcheck-suppress accessMoved
    attributes_hashmap_->GetOrSetDefault(std::move(attr), create_default_aggregation_)
        ->Aggregate(value);
#endif
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
    if (ExemplarFilterEnabled(exemplar_filter_type_, context))
    {
      exemplar_reservoir_->OfferMeasurement(value, {}, context);
    }
#endif
    static MetricAttributes attr = MetricAttributes{};
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
    GetOrCreateEntry(attr)->RecordDouble(value);
#else
    std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
#endif
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes,
                    const opentelemetry::context::Context &context
                    OPENTELEMETRY_MAYBE_UNUSED) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    if (ExemplarFilterEnabled(exemplar_filter_type_, context))
    {
      exemplar_reservoir_->OfferMeasurement(value, attributes, context);
    }
#endif
    MetricAttributes attr{attributes, attributes_processor_.get()};
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
    GetOrCreateEntry(attr)->RecordDouble(value);
#else
    std::lock_guard<std::mutex> guard(attribute_hashmap_lock_);
    // cppcheck-suppress accessMoved
    attributes_hashmap_->GetOrSetDefault(std::move(attr), create_default_aggregation_)
        ->Aggregate(value);
#endif
  }

  bool Collect(CollectorHandle *collector,
               nostd::span<std::shared_ptr<CollectorHandle>> collectors,
               opentelemetry::common::SystemTimestamp sdk_start_ts,
               opentelemetry::common::SystemTimestamp collection_ts,
               nostd::function_ref<bool(MetricData)> callback) noexcept override;

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
  std::shared_ptr<BoundSyncWritableMetricStorage> Bind(
      const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  // Internal: stable bound entry. Self-contained: owns its own mutex and
  // aggregation so the user-held handle stays safe to call even if the parent
  // SyncMetricStorage is destroyed first (writes simply have no observer).
  // Bound and unbound writes share current_ and lock_, preserving recording
  // order even when last-value measurements have identical timestamps.
  // Collect() rotates current_ when dirty and retains user-held entries.
  //
  // Exemplar note: the bound fast path has no per-call Context, so it does not
  // offer measurements to the exemplar reservoir. Callers that need exemplars
  // should use the unbound RecordLong/RecordDouble path.
  class BoundEntry : public BoundSyncWritableMetricStorage
  {
  public:
    BoundEntry(InstrumentValueType value_type,
               std::unique_ptr<Aggregation> initial_aggregation) noexcept
        : value_type_(value_type), current_(std::move(initial_aggregation))
    {}

    void RecordLong(int64_t value) noexcept override;
    void RecordDouble(double value) noexcept override;

  private:
    friend class SyncMetricStorage;
    InstrumentValueType value_type_;
    // Protected by lock_.
    std::mutex lock_;
    std::unique_ptr<Aggregation> current_;
    bool dirty_ = false;
  };
#endif

private:
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
  // Resolve attributes and cardinality under the map lock. The returned owner
  // keeps the entry alive while recording without holding the map lock.
  std::shared_ptr<BoundEntry> GetOrCreateEntry(const MetricAttributes &attributes) noexcept;
#endif

  InstrumentDescriptor instrument_descriptor_;
  const AggregationConfig *aggregation_config_;
#ifndef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
  // Recordings since the last collection when bound instruments are disabled.
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
#endif
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
  std::shared_ptr<const AttributesProcessor> attributes_processor_;
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
  ExemplarFilterType exemplar_filter_type_;
  nostd::shared_ptr<ExemplarReservoir> exemplar_reservoir_;
#endif
  TemporalMetricStorage temporal_metric_storage_;
  // Guards entries_ with the bound preview enabled, attributes_hashmap_ otherwise.
  std::mutex attribute_hashmap_lock_;
#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
  // NOTE: ENABLE_METRICS_BOUND_INSTRUMENTS_PREVIEW changes the layout and
  // vtable of SyncMetricStorage (these conditional members and the virtual
  // Bind() method on SyncWritableMetricStorage). It MUST be defined
  // consistently across the SDK library build and every consumer translation
  // unit, otherwise ODR violations and ABI mismatches will result.
  // One entry per resolved attribute set, shared by bound and unbound writes.
  // The map owns entries for the current interval. Collection releases entries
  // without user-held handles after exporting pending data; retained entries
  // continue to count toward cardinality even during quiet intervals.
  // Guarded by attribute_hashmap_lock_.
  std::unordered_map<MetricAttributes, std::shared_ptr<BoundEntry>, AttributeHashGenerator>
      entries_;
#endif
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
