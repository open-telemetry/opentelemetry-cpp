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
    const std::vector<sdkmetrics::Record> &records) noexcept override;

private:
std::ostream &sout_;
bool isShutdown_ = false;

void PrintVariant(sdkmetrics::RecordValue value, AggregatorKind aggKind)
{
  if(nostd::holds_alternative<std::vector<short>>(value))
  {
    PrintVector<short>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<int>>(value))
  {
    PrintVector<int>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<double>>(value))
  {
    PrintVector<double>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<float>>(value))
  {
    PrintVector<float>(value,aggKind);
  }
}

template <typename T>
void PrintVector(sdkmetrics::RecordValue value, AggregatorKind aggKind)
{
  auto vec = nostd::get<std::vector<T>>(value);
  int size = vec.size();

  if(aggKind == AggregatorKind::Counter)
  {
    sout_ << "\n  sum         : " << vec[0];
  }
  else if(aggKind == AggregatorKind::MinMaxSumCount)
  {
    sout_ << "\n  min         : " << vec[0]
          << "\n  max         : " << vec[1]
          << "\n  sum         : " << vec[2]
          << "\n  count       : " << vec[3];
    
  }
  else if(aggKind == AggregatorKind::Gauge)
  {
    sout_ << "\n  last value  : " << vec[0];
  }
  else if(aggKind == AggregatorKind::Sketch)
  {

  }
  else if(aggKind == AggregatorKind::Histogram)
  {

  }
  else if(aggKind == AggregatorKind::Exact)
  {
    sout_ << "\n  values      : ";
    sout_ <<'[';
    int i = 1;
    for (auto v : vec)
     {
       sout_ << v; 
       if (i != size)
         sout_ << ',';
       i++;
     };
     sout_ << ']';
  }
}

};
}}
OPENTELEMETRY_END_NAMESPACE