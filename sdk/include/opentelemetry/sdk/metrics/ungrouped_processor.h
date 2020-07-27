#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/processor.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
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

    auto find_string_key = batch_map_.find(batch_key);

    if(find_string_key != batch_map_.end())
    {
      auto batch_value = batch_map_[batch_key];
      
      if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto aggregator_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);
        auto batch_value_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(batch_value);

        auto agg = aggregator_reference_short.get();
        auto agg2 = batch_value_reference_short.get();

        agg->merge(agg2);

        aggregator_reference_short = nostd::shared_ptr<sdkmetrics::Aggregator<short>>(agg);
      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
      {

      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
      {

      }
      else if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
      {

      }
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