#include "opentelemetry/exporters/stream/metrics_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
StreamMetricsExporter::StreamMetricsExporter(std::ostream &sout) noexcept
                                    : sout_{sout} {}

sdkmetrics::ExportResult StreamMetricsExporter::Export(
  const std::vector<sdkmetrics::Record> &records) noexcept
{

  if(isShutdown_)
  {
    return sdkmetrics::ExportResult::kFailure;
  }

  for(sdkmetrics::Record record : records)
  {    
    AggregatorKind aggKind = record.GetAggregatorKind();

    sout_ << "{"
          << "\n name        : " << record.GetName()
          << "\n description : " << record.GetDescription()
          << "\n labels      : " << record.GetLabels();
          PrintVariant(record.GetValue(), aggKind);    

    if(aggKind == AggregatorKind::Gauge)
    {
      sout_ << "\n timestamp   : " << record.GetTimestamp();
    }

  }

  return sdkmetrics::ExportResult::kSuccess;
}

void StreamMetricsExporter::Shutdown(std::chrono::microseconds timeout) noexcept 
{
  isShutdown_ = true;
}

} // namespace metrics
} // namespace exporter
OPENTELEMETRY_END_NAMESPACE