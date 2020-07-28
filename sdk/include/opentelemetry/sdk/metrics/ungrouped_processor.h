#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/processor.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
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

    // If we have already seen this instrument labelset pair, we merge with the aggregator we have seen and return
    if(batch_map_.find(batch_key) != batch_map_.end())
    {
      auto batch_value = batch_map_[batch_key];
      
      if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto batch_value_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(batch_value);
        auto aggregator_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);

        auto batch_short = batch_value_reference_short.get();
        auto agg_short = aggregator_reference_short.get();

        batch_short->merge(agg_short);

        // batch_map_[batch_key] = nostd::shared_ptr<sdkmetrics::Aggregator<short>>(agg);
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
      {
        auto batch_value_reference_int = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(batch_value);
        auto aggregator_reference_int = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);

        auto batch_int = batch_value_reference_int.get();
        auto agg_int = aggregator_reference_int.get();

        batch_int->merge(agg_int);
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
      {
        auto batch_value_reference_float = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(batch_value);
        auto aggregator_reference_float = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);

        auto batch_float = batch_value_reference_float.get();
        auto agg_float = aggregator_reference_float.get();

        batch_float->merge(agg_float);
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
      {
        auto batch_value_reference_double = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(batch_value);
        auto aggregator_reference_double = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);

        auto batch_double = batch_value_reference_double.get();
        auto agg_double = aggregator_reference_double.get();

        batch_double->merge(agg_double);
      }
      return;
    }
    // If the processor is stateful and we haven't since this aggregator before
    if(stateful_)
    {      
      if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto aggregator_short = aggregator_for<short>(get_instrument(aggregator));
        auto record_agg_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);

        auto agg_short_raw = aggregator_short.get();
        auto agg_record_raw = record_agg_short.get();

        agg_short_raw->merge(agg_record_raw);

        batch_map_[batch_key] = aggregator_short;
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
      {
        auto aggregator_int = aggregator_for<int>(get_instrument(aggregator));
        auto record_agg_int = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);

        auto agg_int_raw = aggregator_int.get();
        auto record_agg_int_raw = record_agg_int.get();

        agg_int_raw->merge(record_agg_int_raw);

        batch_map_[batch_key] = aggregator_int;      
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
      {
        auto aggregator_float = aggregator_for<float>(get_instrument(aggregator));
        auto record_agg_float = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);

        auto agg_float_raw = aggregator_float.get();
        auto record_agg_float_raw = record_agg_float.get();

        agg_float_raw->merge(record_agg_float_raw);

        batch_map_[batch_key] = aggregator_float;  
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
      {
        auto aggregator_double = aggregator_for<double>(get_instrument(aggregator));
        auto record_agg_double = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);

        auto agg_double_raw = aggregator_double.get();
        auto record_agg_double_raw = record_agg_double.get();

        agg_double_raw->merge(record_agg_double_raw);

        batch_map_[batch_key] = aggregator_double;    
      }
    }
    else {
      batch_map_[batch_key] = aggregator;
    }

  }
private:
  bool stateful_;
  std::unordered_map<std::string, sdkmetrics::AggregatorVariant> batch_map_;

  metrics_api::InstrumentKind get_instrument(sdkmetrics::AggregatorVariant aggregator)
  {
    if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      return nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      return nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
    {
      return nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator)->get_instrument_kind();
    }
    else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      return nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator)->get_instrument_kind();
    }

    return metrics_api::InstrumentKind::Counter;
  }

  template <typename T>
  nostd::shared_ptr<sdkmetrics::Aggregator<T>> aggregator_for(metrics_api::InstrumentKind ins_kind)
  {
    if(ins_kind == metrics_api::InstrumentKind::Counter)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::UpDownCounter)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::ValueRecorder)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::SumObserver)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::UpDownSumObserver)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
    }
    else if(ins_kind == metrics_api::InstrumentKind::ValueObserver)
    {
      return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::MinMaxSumCountAggregator<T>(ins_kind));
    }

    return nostd::shared_ptr<sdkmetrics::Aggregator<T>>(new sdkmetrics::CounterAggregator<T>(ins_kind));
  }

  std::map<metrics_api::InstrumentKind, std::string> ins_to_string {
    {metrics_api::InstrumentKind::Counter,           "Counter"},
    {metrics_api::InstrumentKind::UpDownCounter,     "UpDownCounter"},
    {metrics_api::InstrumentKind::SumObserver,       "SumObserver"},
    {metrics_api::InstrumentKind::UpDownSumObserver, "UpDownSumObserver"},
    {metrics_api::InstrumentKind::ValueObserver,     "ValueObserver"},
    {metrics_api::InstrumentKind::ValueRecorder,     "ValueRecorder"}
  };

  std::map<std::string, metrics_api::InstrumentKind> string_to_ins {
    {"Counter",           metrics_api::InstrumentKind::Counter},
    {"UpDownCounter",     metrics_api::InstrumentKind::UpDownCounter},
    {"SumObserver",       metrics_api::InstrumentKind::SumObserver},
    {"UpDownSumObserver", metrics_api::InstrumentKind::UpDownSumObserver},
    {"ValueObserver",     metrics_api::InstrumentKind::ValueObserver},
    {"ValueRecorder",     metrics_api::InstrumentKind::ValueRecorder}
  };
};
}
}

OPENTELEMETRY_END_NAMESPACE