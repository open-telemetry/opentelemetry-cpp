#pragma once
 
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/sdk/metrics/aggregator/gauge_aggregator.h"
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdkmetrics = opentelemetry::sdk::metrics;

namespace exporter
{
namespace metrics
{

/**
 * The OStreamMetricsExporter exports record data through an ostream
 */
class OStreamMetricsExporter final : public sdkmetrics::MetricsExporter
{
public:

/**
 * Create an OStreamMetricsExporter. This constructor takes in a reference to an ostream that the
 * export() function will send span data into.
 * The default ostream is set to stdout
 */
explicit OStreamMetricsExporter(std::ostream &sout = std::cout) noexcept;

sdkmetrics::ExportResult Export(
    std::vector<sdkmetrics::Record> &records) noexcept override;

private:
std::ostream &sout_;


template <typename T>
void PrintAggregatorVariant(sdkmetrics::AggregatorVariant value)
{
  auto agg = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<T>>>(value);
  auto aggKind = agg->get_aggregator_kind();

  if(aggKind == sdkmetrics::AggregatorKind::Counter)
  {
    sout_ << "\n  sum         : " << agg->get_checkpoint()[0];
  }
  else if(aggKind == sdkmetrics::AggregatorKind::MinMaxSumCount)
  {
    auto mmsc = agg->get_checkpoint();
    sout_ << "\n  min         : " << mmsc[0]
          << "\n  max         : " << mmsc[1]
          << "\n  sum         : " << mmsc[2]
          << "\n  count       : " << mmsc[3];
  }
  else if(aggKind == sdkmetrics::AggregatorKind::Gauge)
  {
    // Since Aggregator doesn't have GaugeAggregator specific functions, we need to cast to GaugeAggregator
    std::shared_ptr<opentelemetry::sdk::metrics::GaugeAggregator<T>> temp_gauge;
    temp_gauge = std::dynamic_pointer_cast<opentelemetry::sdk::metrics::GaugeAggregator<T>>(agg);
    auto timestamp = temp_gauge->get_checkpoint_timestamp();

    sout_ << "\n  last value  : " << temp_gauge->get_checkpoint()[0]
          << "\n  timestamp   : " << std::to_string(timestamp.time_since_epoch().count());
  }
  else if(aggKind == sdkmetrics::AggregatorKind::Exact)
  {

  }
}

};
}
}
OPENTELEMETRY_END_NAMESPACE