#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/processor.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/exact_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/gauge_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/histogram_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/sketch_aggregator.h"
#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{

namespace metrics
{
class UngroupedMetricsProcessor : public MetricsProcessor
{
public:
  explicit UngroupedMetricsProcessor(bool stateful)
  {
    stateful_ = stateful;
  }

  /**
   * CheckpointSelf will return a vector of records to be exported. This function will go through aggregators
   * that have been sent through process() and return them as a vector of records.
   **/
  std::vector<sdkmetrics::Record> CheckpointSelf() noexcept
  {
    std::vector<sdkmetrics::Record> metric_records;
    
    for(auto iter : batch_map_)
    {
      std::string key = iter.first;
      std::size_t description_index = key.find("/description/");
      std::size_t labels_index = key.find("/labels/");
      std::size_t instrument_index = key.find("/instrument/");

      std::string name = key.substr(6, description_index - 6);
      std::string description = key.substr(description_index + 13, labels_index - description_index - 13);
      std::string labels = key.substr(labels_index + 8, instrument_index - labels_index - 8);
      
      sdkmetrics::Record r{name, description, labels, iter.second};

      metric_records.push_back(r);
    }

    return metric_records;
  }

  /**
   * Once Process is called, FinishCollection() should also be called. In the case of a non stateful processor
   * the map will be reset.
   **/
  void FinishedCollection() noexcept
  {
    if(!stateful_)
    {
      batch_map_ = {};
    }
  }

  void process(sdkmetrics::Record record) noexcept
  {
    auto aggregator = record.GetAggregator();
    std::string label = record.GetLabels();
    std::string name = record.GetName();
    std::string description = record.GetDescription();
    std::string instrument = ins_to_string[get_instrument(aggregator)];

    std::string batch_key = "/name/" + name + "/description/" + description + "/labels/" + label + "/instrument/" + instrument; 

    /**
     * If we have already seen this aggregator then we will merge it with the copy that exists in the batch_map_
     * The call to merge here combines only identical records (same key)
     **/
    if(batch_map_.find(batch_key) != batch_map_.end())
    {
      auto batch_value = batch_map_[batch_key];
      
      if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto batch_value_reference_short = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(batch_value);
        auto aggregator_reference_short = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);

        merge_aggregators<short>(batch_value_reference_short, aggregator_reference_short);
      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
      {
        auto batch_value_reference_int = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(batch_value);
        auto aggregator_reference_int = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);

        merge_aggregators<int>(batch_value_reference_int, aggregator_reference_int);
      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
      {
        auto batch_value_reference_float = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(batch_value);
        auto aggregator_reference_float = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);

        merge_aggregators<float>(batch_value_reference_float, aggregator_reference_float);

      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
      {
        auto batch_value_reference_double = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(batch_value);
        auto aggregator_reference_double = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);

        merge_aggregators<double>(batch_value_reference_double, aggregator_reference_double);
      }
      return;
    }
    /**
     * If the processor is stateful and this aggregator has not be seen by the processor yet.
     * We create a copy of this aggregator, merge it with the aggregator from the given record.
     * Then set this copied aggregator with the batch_key in the batch_map_
     **/
    if(stateful_)
    {      
      if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto record_agg_short = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);
        auto aggregator_short = aggregator_copy<short>(record_agg_short);

        merge_aggregators<short>(aggregator_short, record_agg_short);

        batch_map_[batch_key] = aggregator_short;
      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
      {
        auto record_agg_int = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);
        auto aggregator_int = aggregator_copy<int>(record_agg_int);

        merge_aggregators<int>(aggregator_int, record_agg_int);

        batch_map_[batch_key] = aggregator_int;      
      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
      {
        auto record_agg_float = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);
        auto aggregator_float = aggregator_copy<float>(record_agg_float);

        merge_aggregators<float>(aggregator_float, record_agg_float);

        batch_map_[batch_key] = aggregator_float;  
      }
      else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
      {
        auto record_agg_double = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);
        auto aggregator_double = aggregator_copy<double>(record_agg_double);

        merge_aggregators<double>(aggregator_double, record_agg_double);

        batch_map_[batch_key] = aggregator_double;    
      }
    }
    else 
    {
      /**
       * If the processor is not stateful, we don't need to create a copy of the aggregator, since the map will be
       * reset from FinishedCollection().
       **/
      batch_map_[batch_key] = aggregator;
    }

  }
private:
  bool stateful_;
  std::unordered_map<std::string, sdkmetrics::AggregatorVariant> batch_map_;

  metrics_api::InstrumentKind get_instrument(sdkmetrics::AggregatorVariant aggregator)
  {
    if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      return nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator)->get_instrument_kind();
    }

    return metrics_api::InstrumentKind::Counter;
  }

  template <typename T>
  std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator_copy(std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator)
  {
    auto ins_kind = aggregator->get_instrument_kind();
    auto agg_kind = aggregator->get_aggregator_kind();

    if(agg_kind == sdkmetrics::AggregatorKind::Counter)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::MinMaxSumCount)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Gauge)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::GaugeAggregator<T>(ins_kind));
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Sketch)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::SketchAggregator<T>(ins_kind, 
                                                          aggregator->get_error_bound(), aggregator->get_max_buckets()));
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Histogram)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::HistogramAggregator<T>(ins_kind, aggregator->get_boundaries()));
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Exact)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::ExactAggregator<T>(ins_kind, aggregator->get_quant_estimation()));
    }

    return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
  };

  template <typename T>
  std::shared_ptr<sdkmetrics::Aggregator<T>> aggregator_for(metrics_api::InstrumentKind ins_kind)
  {
    if(ins_kind == metrics_api::InstrumentKind::Counter)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::UpDownCounter)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::ValueRecorder)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::SumObserver)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::UpDownSumObserver)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::ValueObserver)
    {
      return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));
    }

    return std::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
  };

  template <typename T>
  void merge_aggregators(std::shared_ptr<sdkmetrics::Aggregator<T>> batch_agg, 
                         std::shared_ptr<sdkmetrics::Aggregator<T>> record_agg)
  {
    auto agg_kind = batch_agg->get_aggregator_kind();
    if(agg_kind == sdkmetrics::AggregatorKind::Counter)
    {
      std::shared_ptr<sdkmetrics::CounterAggregator<T>> temp_batch_agg_counter = 
        std::dynamic_pointer_cast<sdkmetrics::CounterAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::CounterAggregator<T>> temp_record_agg_counter = 
        std::dynamic_pointer_cast<sdkmetrics::CounterAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_counter = temp_batch_agg_counter.get();
      auto temp_record_agg_raw_counter = temp_record_agg_counter.get();
      
      temp_batch_agg_raw_counter->merge(*temp_record_agg_raw_counter);
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::MinMaxSumCount)
    {
      std::shared_ptr<sdkmetrics::MinMaxSumCountAggregator<T>> temp_batch_agg_mmsc = 
        std::dynamic_pointer_cast<sdkmetrics::MinMaxSumCountAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::MinMaxSumCountAggregator<T>> temp_record_agg_mmsc = 
        std::dynamic_pointer_cast<sdkmetrics::MinMaxSumCountAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_mmsc = temp_batch_agg_mmsc.get();
      auto temp_record_agg_raw_mmsc = temp_record_agg_mmsc.get();
      
      temp_batch_agg_raw_mmsc->merge(*temp_record_agg_raw_mmsc);
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Gauge)
    {
      std::shared_ptr<sdkmetrics::GaugeAggregator<T>> temp_batch_agg_gauge = 
        std::dynamic_pointer_cast<sdkmetrics::GaugeAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::GaugeAggregator<T>> temp_record_agg_gauge = 
        std::dynamic_pointer_cast<sdkmetrics::GaugeAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_gauge = temp_batch_agg_gauge.get();
      auto temp_record_agg_raw_gauge = temp_record_agg_gauge.get();
      
      temp_batch_agg_raw_gauge->merge(*temp_record_agg_raw_gauge);
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Sketch)
    {
      std::shared_ptr<sdkmetrics::SketchAggregator<T>> temp_batch_agg_sketch = 
        std::dynamic_pointer_cast<sdkmetrics::SketchAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::SketchAggregator<T>> temp_record_agg_sketch = 
        std::dynamic_pointer_cast<sdkmetrics::SketchAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_sketch = temp_batch_agg_sketch.get();
      auto temp_record_agg_raw_sketch = temp_record_agg_sketch.get();
      
      temp_batch_agg_raw_sketch->merge(*temp_record_agg_raw_sketch);
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Histogram)
    {
      std::shared_ptr<sdkmetrics::HistogramAggregator<T>> temp_batch_agg_histogram = 
        std::dynamic_pointer_cast<sdkmetrics::HistogramAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::HistogramAggregator<T>> temp_record_agg_histogram = 
        std::dynamic_pointer_cast<sdkmetrics::HistogramAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_histogram = temp_batch_agg_histogram.get();
      auto temp_record_agg_raw_histogram = temp_record_agg_histogram.get();
      
      temp_batch_agg_raw_histogram->merge(*temp_record_agg_raw_histogram);
    }
    else if(agg_kind == sdkmetrics::AggregatorKind::Exact)
    {
      std::shared_ptr<sdkmetrics::ExactAggregator<T>> temp_batch_agg_exact = 
        std::dynamic_pointer_cast<sdkmetrics::ExactAggregator<T>> (batch_agg);

      std::shared_ptr<sdkmetrics::ExactAggregator<T>> temp_record_agg_exact = 
        std::dynamic_pointer_cast<sdkmetrics::ExactAggregator<T>> (record_agg);

      auto temp_batch_agg_raw_exact = temp_batch_agg_exact.get();
      auto temp_record_agg_raw_exact = temp_record_agg_exact.get();
      
      temp_batch_agg_raw_exact->merge(*temp_record_agg_raw_exact);
    }
  }

  std::map<metrics_api::InstrumentKind, std::string> ins_to_string {
    {metrics_api::InstrumentKind::Counter,           "Counter"},
    {metrics_api::InstrumentKind::UpDownCounter,     "UpDownCounter"},
    {metrics_api::InstrumentKind::SumObserver,       "SumObserver"},
    {metrics_api::InstrumentKind::UpDownSumObserver, "UpDownSumObserver"},
    {metrics_api::InstrumentKind::ValueObserver,     "ValueObserver"},
    {metrics_api::InstrumentKind::ValueRecorder,     "ValueRecorder"}
  };

};
}
}

OPENTELEMETRY_END_NAMESPACE