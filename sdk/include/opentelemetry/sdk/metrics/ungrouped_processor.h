#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/processor.h"
#include "opentelemetry/sdk/metrics/record.h"

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

    std::string batch_key = "/name/" + name + "/description/" + description + "/labels/" + label; 

    auto find_string_key = batch_map_.find(batch_key);

    if(find_string_key != batch_map_.end())
    {
      auto batch_value = batch_map_[batch_key];
      
      if(nostd::holds_alternative<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
      {
        auto aggregator_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);
        auto batch_value_reference_short = nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(batch_value);

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
};
}
}

OPENTELEMETRY_END_NAMESPACE