#pragma once
 
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/exporter.h"
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
void PrintVector(sdkmetrics::AggregatorVariant value)
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
    sout_ << "\n  min         : " << mmsc[0];
  }
}


void GetAggType(sdkmetrics::AggregatorVariant value)
{
  if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(value))
  {
    PrintVector<int>(value);
  }
  else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(value))
  {
    PrintVector<short>(value);
  }
  else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(value))
  {
    PrintVector<double>(value);
  }
  else if(nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(value))
  {
    PrintVector<float>(value);
  }

}

};
}
}
OPENTELEMETRY_END_NAMESPACE