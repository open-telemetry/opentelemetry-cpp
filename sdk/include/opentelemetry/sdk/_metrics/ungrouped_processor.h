// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <map>
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/exact_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/gauge_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/histogram_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/sketch_aggregator.h"
#  include "opentelemetry/sdk/_metrics/processor.h"
#  include "opentelemetry/sdk/_metrics/record.h"
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
  opentelemetry::metrics::InstrumentKind ins_kind;

  // constructor
  KeyStruct(std::string name,
            std::string description,
            std::string labels,
            opentelemetry::metrics::InstrumentKind ins_kind)
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

  std::vector<opentelemetry::sdk::metrics::Record> CheckpointSelf() noexcept override;

  virtual void FinishedCollection() noexcept override;

  virtual void process(opentelemetry::sdk::metrics::Record record) noexcept override;

private:
  bool stateful_;
  std::unordered_map<KeyStruct, opentelemetry::sdk::metrics::AggregatorVariant, KeyStruct_Hash>
      batch_map_;

  /**
   * get_instrument returns the instrument from the passed in AggregatorVariant. We have to
   * unpack the variant then get the instrument from the Aggreagtor.
   */
  opentelemetry::metrics::InstrumentKind get_instrument(
      opentelemetry::sdk::metrics::AggregatorVariant aggregator)
  {
    if (nostd::holds_alternative<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(
            aggregator))
    {
      return nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator)
          ->get_instrument_kind();
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(
                 aggregator)
          ->get_instrument_kind();
    }

    return opentelemetry::metrics::InstrumentKind::Counter;
  }

  /**
   * aggregator_copy creates a copy of the aggregtor passed through process() for a
   * stateful processor. For Sketch, Histogram and Exact we also need to pass in
   * additional constructor values
   */
  template <typename T>
  std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> aggregator_copy(
      std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> aggregator)
  {
    auto ins_kind = aggregator->get_instrument_kind();
    auto agg_kind = aggregator->get_aggregator_kind();

    switch (agg_kind)
    {
      case opentelemetry::sdk::metrics::AggregatorKind::Counter:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));

      case opentelemetry::sdk::metrics::AggregatorKind::MinMaxSumCount:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>(ins_kind));

      case opentelemetry::sdk::metrics::AggregatorKind::Gauge:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::GaugeAggregator<T>(ins_kind));

      case opentelemetry::sdk::metrics::AggregatorKind::Sketch:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::SketchAggregator<T>(
                ins_kind, aggregator->get_error_bound(), aggregator->get_max_buckets()));

      case opentelemetry::sdk::metrics::AggregatorKind::Histogram:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::HistogramAggregator<T>(ins_kind,
                                                                    aggregator->get_boundaries()));

      case opentelemetry::sdk::metrics::AggregatorKind::Exact:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::ExactAggregator<T>(
                ins_kind, aggregator->get_quant_estimation()));

      default:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));
    }
  };

  /**
   * aggregator_for will return an Aggregator based off the instrument passed in. This should be
   * the function that we assign Aggreagtors for instruments, but is currently unused in our
   * pipeline.
   */
  template <typename T>
  std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> aggregator_for(
      opentelemetry::metrics::InstrumentKind ins_kind)
  {
    switch (ins_kind)
    {
      case opentelemetry::metrics::InstrumentKind::Counter:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));

      case opentelemetry::metrics::InstrumentKind::UpDownCounter:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));

      case opentelemetry::metrics::InstrumentKind::ValueRecorder:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>(ins_kind));

      case opentelemetry::metrics::InstrumentKind::SumObserver:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));

      case opentelemetry::metrics::InstrumentKind::UpDownSumObserver:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));

      case opentelemetry::metrics::InstrumentKind::ValueObserver:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>(ins_kind));

      default:
        return std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>>(
            new opentelemetry::sdk::metrics::CounterAggregator<T>(ins_kind));
    }
  };

  /**
   * merge_aggreagtors takes in two shared pointers to aggregators of the same kind.
   * We first need to dynamically cast to the actual Aggregator that is held in the
   * Aggregator<T> wrapper. Then we must get the underlying pointer from the shared
   * pointer and merge them together.
   */
  template <typename T>
  void merge_aggregators(std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> batch_agg,
                         std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> record_agg)
  {
    auto agg_kind = batch_agg->get_aggregator_kind();
    if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::Counter)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::CounterAggregator<T>> temp_batch_agg_counter =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::CounterAggregator<T>>(batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::CounterAggregator<T>> temp_record_agg_counter =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::CounterAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_counter  = temp_batch_agg_counter.get();
      auto temp_record_agg_raw_counter = temp_record_agg_counter.get();

      temp_batch_agg_raw_counter->merge(*temp_record_agg_raw_counter);
    }
    else if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::MinMaxSumCount)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>>
          temp_batch_agg_mmsc =
              std::dynamic_pointer_cast<opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>>(
                  batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>>
          temp_record_agg_mmsc =
              std::dynamic_pointer_cast<opentelemetry::sdk::metrics::MinMaxSumCountAggregator<T>>(
                  record_agg);

      auto temp_batch_agg_raw_mmsc  = temp_batch_agg_mmsc.get();
      auto temp_record_agg_raw_mmsc = temp_record_agg_mmsc.get();

      temp_batch_agg_raw_mmsc->merge(*temp_record_agg_raw_mmsc);
    }
    else if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::Gauge)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::GaugeAggregator<T>> temp_batch_agg_gauge =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::GaugeAggregator<T>>(batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::GaugeAggregator<T>> temp_record_agg_gauge =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::GaugeAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_gauge  = temp_batch_agg_gauge.get();
      auto temp_record_agg_raw_gauge = temp_record_agg_gauge.get();

      temp_batch_agg_raw_gauge->merge(*temp_record_agg_raw_gauge);
    }
    else if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::Sketch)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::SketchAggregator<T>> temp_batch_agg_sketch =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::SketchAggregator<T>>(batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::SketchAggregator<T>> temp_record_agg_sketch =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::SketchAggregator<T>>(record_agg);

      auto temp_batch_agg_raw_sketch  = temp_batch_agg_sketch.get();
      auto temp_record_agg_raw_sketch = temp_record_agg_sketch.get();

      temp_batch_agg_raw_sketch->merge(*temp_record_agg_raw_sketch);
    }
    else if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::Histogram)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::HistogramAggregator<T>>
          temp_batch_agg_histogram =
              std::dynamic_pointer_cast<opentelemetry::sdk::metrics::HistogramAggregator<T>>(
                  batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::HistogramAggregator<T>>
          temp_record_agg_histogram =
              std::dynamic_pointer_cast<opentelemetry::sdk::metrics::HistogramAggregator<T>>(
                  record_agg);

      auto temp_batch_agg_raw_histogram  = temp_batch_agg_histogram.get();
      auto temp_record_agg_raw_histogram = temp_record_agg_histogram.get();

      temp_batch_agg_raw_histogram->merge(*temp_record_agg_raw_histogram);
    }
    else if (agg_kind == opentelemetry::sdk::metrics::AggregatorKind::Exact)
    {
      std::shared_ptr<opentelemetry::sdk::metrics::ExactAggregator<T>> temp_batch_agg_exact =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::ExactAggregator<T>>(batch_agg);

      std::shared_ptr<opentelemetry::sdk::metrics::ExactAggregator<T>> temp_record_agg_exact =
          std::dynamic_pointer_cast<opentelemetry::sdk::metrics::ExactAggregator<T>>(record_agg);

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
