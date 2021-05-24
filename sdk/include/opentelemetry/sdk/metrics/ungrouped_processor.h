// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <map>
#  include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/exact_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/gauge_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/histogram_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/sketch_aggregator.h"
#  include "opentelemetry/sdk/metrics/processor.h"
#  include "opentelemetry/sdk/metrics/record.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{

namespace metrics
{

struct KeyStruct
{
  std::string name;
  std::string description;
  std::string labels;
  metrics_api::InstrumentKind ins_kind;

  // constructor
  KeyStruct(std::string name,
            std::string description,
            std::string labels,
            metrics_api::InstrumentKind ins_kind)
  {
    this->name        = name;
    this->description = description;
    this->labels      = labels;
    this->ins_kind    = ins_kind;
  }

  // operator== is required to compare keys in case of hash collision
  bool operator==(const KeyStruct &p) const
  {
    return name == p.name && description == p.description && labels == p.labels &&
           ins_kind == p.ins_kind;
  }
};

struct KeyStruct_Hash
{
  std::size_t operator()(const KeyStruct &keystruct) const
  {
    std::size_t name_size   = keystruct.name.length();
    std::size_t desc_size   = keystruct.description.length();
    std::size_t labels_size = keystruct.labels.length();
    std::size_t ins_size    = (int)keystruct.ins_kind;

    return (name_size ^ desc_size ^ labels_size) + ins_size;
  }
};

class UngroupedMetricsProcessor : public MetricsProcessor
{
public:
  explicit UngroupedMetricsProcessor(bool stateful);

  std::vector<sdkmetrics::Record> CheckpointSelf() noexcept override;

  virtual void FinishedCollection() noexcept override;

  virtual void process(sdkmetrics::Record record) noexcept override;

private:
  bool stateful_;
  std::unordered_map<KeyStruct, sdkmetrics::AggregatorVariant, KeyStruct_Hash> batch_map_;

  /**
   * get_instrument returns the instrument from the passed in AggregatorVariant. We have to
   * unpack the variant then get the instrument from the Aggreagtor.
   */
  metrics_api::InstrumentKind get_instrument(sdkmetrics::AggregatorVariant aggregator)
  {
    if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator)
          ->get_instrument_kind();
    }

    return metrics_api::InstrumentKind::Counter;
  }

  /**
   * aggregator_copy creates a copy of the aggregtor passed through process() for a
   * stateful processor. For Sketch, Histogram and Exact we also need to pass in
   * additional constructor values
   */
  template <typename T>
  std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator_copy(
      std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator)
  {
    auto ins_kind = aggregator->get_instrument_kind();
    auto agg_kind = aggregator->get_aggregator_kind();

    switch (agg_kind)
    {
      case sdkmetrics::AggregatorKind::Counter:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));

      case sdkmetrics::AggregatorKind::MinMaxSumCount:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));

      case sdkmetrics::AggregatorKind::Gauge:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::GaugeAggregator<T>(ins_kind));

      case sdkmetrics::AggregatorKind::Sketch:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::SketchAggregator<T>(
            ins_kind, aggregator->get_error_bound(), aggregator->get_max_buckets()));

      case sdkmetrics::AggregatorKind::Histogram:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::HistogramAggregator<T>(ins_kind, aggregator->get_boundaries()));

      case sdkmetrics::AggregatorKind::Exact:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::ExactAggregator<T>(ins_kind, aggregator->get_quant_estimation()));

      default:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
  };

  /**
   * aggregator_for will return an Aggregator based off the instrument passed in. This should be
   * the function that we assign Aggreagtors for instruments, but is currently unused in our
   * pipeline.
   */
  template <typename T>
  std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator_for(metrics_api::InstrumentKind ins_kind)
  {
    switch (ins_kind)
    {
      case metrics_api::InstrumentKind::Counter:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));

      case metrics_api::InstrumentKind::UpDownCounter:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));

      case metrics_api::InstrumentKind::ValueRecorder:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));

      case metrics_api::InstrumentKind::SumObserver:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));

      case metrics_api::InstrumentKind::UpDownSumObserver:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));

      case metrics_api::InstrumentKind::ValueObserver:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));

      default:
        return std::shared_ptr<sdkmetrics::Aggregator<T>>(
            new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
  };

  /**
   * merge_aggreagtors takes in two shared pointers to aggregators of the same kind.
   * We first need to dynamically cast to the actual Aggregator that is held in the
   * Aggregator<T> wrapper. Then we must get the underlying pointer from the shared
   * pointer and merge them together.
   */
  template <typename T>
  void merge_aggregators(std::shared_ptr<sdkmetrics::Aggregator<T>> batch_agg,
                         std::shared_ptr<sdkmetrics::Aggregator<T>> record_agg)
  {
    auto agg_kind = batch_agg->get_aggregator_kind();
    if (agg_kind == sdkmetrics::AggregatorKind::Counter)
    {
      std::shared_ptr<sdkmetrics::CounterAggregator<T>> temp_batch_agg_counter =
          std::dynamic_pointer_cast<sdkmetrics::CounterAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::CounterAggregator<T>> temp_record_agg_counter =
          std::dynamic_pointer_cast<sdkmetrics::CounterAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_counter  = temp_batch_agg_counter.get();
      auto temp_record_agg_raw_counter = temp_record_agg_counter.get();

      temp_batch_agg_raw_counter->merge(*temp_record_agg_raw_counter);
    }
    else if (agg_kind == sdkmetrics::AggregatorKind::MinMaxSumCount)
    {
      std::shared_ptr<sdkmetrics::MinMaxSumCountAggregator<T>> temp_batch_agg_mmsc =
          std::dynamic_pointer_cast<sdkmetrics::MinMaxSumCountAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::MinMaxSumCountAggregator<T>> temp_record_agg_mmsc =
          std::dynamic_pointer_cast<sdkmetrics::MinMaxSumCountAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_mmsc  = temp_batch_agg_mmsc.get();
      auto temp_record_agg_raw_mmsc = temp_record_agg_mmsc.get();

      temp_batch_agg_raw_mmsc->merge(*temp_record_agg_raw_mmsc);
    }
    else if (agg_kind == sdkmetrics::AggregatorKind::Gauge)
    {
      std::shared_ptr<sdkmetrics::GaugeAggregator<T>> temp_batch_agg_gauge =
          std::dynamic_pointer_cast<sdkmetrics::GaugeAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::GaugeAggregator<T>> temp_record_agg_gauge =
          std::dynamic_pointer_cast<sdkmetrics::GaugeAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_gauge  = temp_batch_agg_gauge.get();
      auto temp_record_agg_raw_gauge = temp_record_agg_gauge.get();

      temp_batch_agg_raw_gauge->merge(*temp_record_agg_raw_gauge);
    }
    else if (agg_kind == sdkmetrics::AggregatorKind::Sketch)
    {
      std::shared_ptr<sdkmetrics::SketchAggregator<T>> temp_batch_agg_sketch =
          std::dynamic_pointer_cast<sdkmetrics::SketchAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::SketchAggregator<T>> temp_record_agg_sketch =
          std::dynamic_pointer_cast<sdkmetrics::SketchAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_sketch  = temp_batch_agg_sketch.get();
      auto temp_record_agg_raw_sketch = temp_record_agg_sketch.get();

      temp_batch_agg_raw_sketch->merge(*temp_record_agg_raw_sketch);
    }
    else if (agg_kind == sdkmetrics::AggregatorKind::Histogram)
    {
      std::shared_ptr<sdkmetrics::HistogramAggregator<T>> temp_batch_agg_histogram =
          std::dynamic_pointer_cast<sdkmetrics::HistogramAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::HistogramAggregator<T>> temp_record_agg_histogram =
          std::dynamic_pointer_cast<sdkmetrics::HistogramAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_histogram  = temp_batch_agg_histogram.get();
      auto temp_record_agg_raw_histogram = temp_record_agg_histogram.get();

      temp_batch_agg_raw_histogram->merge(*temp_record_agg_raw_histogram);
    }
    else if (agg_kind == sdkmetrics::AggregatorKind::Exact)
    {
      std::shared_ptr<sdkmetrics::ExactAggregator<T>> temp_batch_agg_exact =
          std::dynamic_pointer_cast<sdkmetrics::ExactAggregator<T>>(batch_agg);

      std::shared_ptr<sdkmetrics::ExactAggregator<T>> temp_record_agg_exact =
          std::dynamic_pointer_cast<sdkmetrics::ExactAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_exact  = temp_batch_agg_exact.get();
      auto temp_record_agg_raw_exact = temp_record_agg_exact.get();

      temp_batch_agg_raw_exact->merge(*temp_record_agg_raw_exact);
    }
  }
};
}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
