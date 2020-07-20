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
 * The StreamMetricsExporter exports record data through an ostream
 */
class StreamMetricsExporter final : public sdkmetrics::MetricsExporter
{
public:

/**
 * Create a StreamMetricsExporter. This constructor takes in a reference to an ostream that the
 * export() function will send span data into.
 * The default ostream is set to stdout
 */
explicit StreamMetricsExporter(std::ostream &sout = std::cout) noexcept;

sdkmetrics::ExportResult Export(
    const std::vector<sdkmetrics::Record> &records) noexcept override;

void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept;

private:
std::ostream &sout_;
bool isShutdown_ = false;

void PrintVariant(sdkmetrics::RecordValue value, AggregatorKind aggKind)
{
  if(nostd::holds_alternative<std::vector<short>>(value))
  {
    PrintVector<std::vector<short>>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<int>>(value))
  {
    PrintVector<std::vector<int>>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<double>>(value))
  {
    PrintVector<std::vector<double>>(value,aggKind);
  }
  else if(nostd::holds_alternative<std::vector<float>>(value))
  {
    PrintVector<std::vector<float>>(value,aggKind);
  }
}

template <typename T>
void PrintVector(sdkmetrics::RecordValue value, AggregatorKind aggKind)
{

}

};
}}
OPENTELEMETRY_END_NAMESPACE