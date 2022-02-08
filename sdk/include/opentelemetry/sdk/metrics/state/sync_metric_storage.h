// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#  include "opentelemetry/sdk/metrics/view/view.h"
#  include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class SyncMetricStorage : public MetricStorage, public WritableMetricStorage
{

public:
  SyncMetricStorage(
      InstrumentDescriptor instrument_descriptor,
      const AggregationType aggregation_type,
      const AttributesProcessor *attributes_processor = new DefaultAttributesProcessor())
      : aggregation_type_{aggregation_type},
        instrument_descriptor_{instrument_descriptor},
        attributes_processor_{attributes_processor}
  {}

  void RecordLong(long value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    size_t hashcode = 0;  // for empty attributs
    if (attributes_aggregation_.find(hashcode) == attributes_aggregation_.end())
    {
      attributes_aggregation_[hashcode] = std::move(create_aggregation());
    }
    attributes_aggregation_[hashcode]->Aggregate(value);
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    auto attr     = attributes_processor_->process(attributes);
    auto hashcode = opentelemetry::sdk::common::GetHashForAttributeMap(attr);
    if (attributes_aggregation_.find(hashcode) == attributes_aggregation_.end())
    {
      attributes_aggregation_[hashcode] = std::move(create_aggregation());
    }
    attributes_aggregation_[hashcode]->Aggregate(value);
  }

  void RecordDouble(double value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }
    size_t hashcode = 0;  // for empty attributs
    if (attributes_aggregation_.find(hashcode) == attributes_aggregation_.end())
    {
      attributes_aggregation_[hashcode] = std::move(create_aggregation());
    }
    attributes_aggregation_[hashcode]->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }

    auto attr     = attributes_processor_->process(attributes);
    auto hashcode = opentelemetry::sdk::common::GetHashForAttributeMap(attr);
    if (attributes_aggregation_.find(hashcode) == attributes_aggregation_.end())
    {
      attributes_aggregation_[hashcode] = std::move(create_aggregation());
    }
    attributes_aggregation_[hashcode]->Aggregate(value);
  }

  bool Collect(AggregationTemporarily aggregation_temporarily,
               nostd::function_ref<bool(MetricData)> callback) noexcept override
  {
    if (callback(MetricData()))
    {
      return true;
    }
    return false;
  }

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;
  std::unordered_map<size_t, std::unique_ptr<Aggregation>> attributes_aggregation_;
  const AttributesProcessor *attributes_processor_;

  std::unique_ptr<Aggregation> create_aggregation()
  {
    switch (aggregation_type_)
    {
      case AggregationType::kDrop:
        return std::move(std::unique_ptr<Aggregation>(new DropAggregation()));
        break;
      case AggregationType::kHistogram:
        if (instrument_descriptor_.value_type_ == InstrumentValueType::kLong)
        {
          return std::move(std::unique_ptr<Aggregation>(new LongHistogramAggregation()));
        }
        else
        {
          return std::move(std::unique_ptr<Aggregation>(new DoubleHistogramAggregation()));
        }
        break;
      case AggregationType::kLastValue:
        if (instrument_descriptor_.value_type_ == InstrumentValueType::kLong)
        {
          return std::move(std::unique_ptr<Aggregation>(new LongLastValueAggregation()));
        }
        else
        {
          return std::move(std::unique_ptr<Aggregation>(new DoubleLastValueAggregation()));
        }
        break;
      case AggregationType::kSum:
        if (instrument_descriptor_.value_type_ == InstrumentValueType::kLong)
        {
          return std::move(std::unique_ptr<Aggregation>(new LongSumAggregation(true)));
        }
        else
        {
          return std::move(std::unique_ptr<Aggregation>(new DoubleSumAggregation(true)));
        }
        break;
      default:
        return std::move(DefaultAggregation::CreateAggregation(instrument_descriptor_));
    }
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif