// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"

#  include "opentelemetry/sdk/metrics/state/temporal_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <list>
#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
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
        exemplar_reservoir_(exemplar_reservoir),
        temporal_metric_storage_(instrument_descriptor)

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
               nostd::function_ref<bool(MetricData)> callback) noexcept override;

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;

  // hashmap to maintain the metrics for delta collection (i.e, collection since last Collect call)
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
  const AttributesProcessor *attributes_processor_;
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
  nostd::shared_ptr<ExemplarReservoir> exemplar_reservoir_;
  TemporalMetricStorage temporal_metric_storage_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif