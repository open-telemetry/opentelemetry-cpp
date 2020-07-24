#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{

namespace metrics
{
class UngroupedMetricsProcessor : public MetricsProcessor
{
  void process(sdkmetrics::Record record) noexcept
  {
    auto agg = record.GetAggregator();
  }

};
}
}