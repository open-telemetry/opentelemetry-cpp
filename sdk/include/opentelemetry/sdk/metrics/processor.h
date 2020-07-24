#pragma once
#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/record.h"

#include <unordered_map>
#include <string>

namespace sdkmetrics = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{
typedef std::tuple<std::string, std::string, std::string,
          opentelemetry::metrics::InstrumentKind,> instrument_label_tuple;

class Processor
{
  Processor(bool stateful)
  {
    stateful_ = stateful;
    batch_map_ =  {};
  }

  std::vector<sdkmetrics::Record> CheckpointSelf() noexcept
  {
    std::vector<sdkmetrics::Record> metric_records;
    
    for(auto iter : batch_map_)
    {
      instrument_label_tuple tuple = iter.first;
      nostd::string_view name = std::get<0>(tuple);
      nostd::string_view description = std::get<1>(tuple);
      std::string labels = std::get<2>(tuple);

      sdkmetrics::Record r(name, description, labels, iter.second);
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

  virtual void process(sdkmetrics::Record) noexcept = 0;




private:
  bool stateful_;
  std::unordered_map<instrument_label_tuple, sdkmetrics::AggregatorVariant> batch_map_;
};

}
}

OPENTELEMETRY_END_NAMESPACE