#pragma once
#include "opentelemetry/version.h"

#include <map>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{

class Processor
{
  Processor(bool stateful)
  {
    stateful_ = stateful;
    batch_map_ =  {};
  }

  static Aggregator AggregatorFor(InstrumentType instrument_type)
  {
    if(instrument_type == InstrumentType::Counter || instrument_type == InstrumentType::UpDownCounter)
    {
      return CounterAggregator();
    }
    else if(instrument_type == InstrumentType::SumObserver || instrument_type == InstrumentType::UpDownSumObserver)
    {
      return GaugeAggregator();
    }
    else if(instrument_type == InstrumentType::ValueRecorder || instrument_type == InstrumentType::ValueObserver)
    {
      return MinMaxSumCountAggreagtor();
    }

  }

private:
  bool stateful_;
  unordered_map<std::string, std::string> batch_map_;
};

}
}

OPENTELEMETRY_END_NAMESPACE