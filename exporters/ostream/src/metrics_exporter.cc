#include "opentelemetry/exporters/ostream/metrics_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

OStreamMetricsExporter::OStreamMetricsExporter(std::ostream &sout) noexcept
                                    : sout_{sout} {}

sdkmetrics::ExportResult OStreamMetricsExporter::Export(
  std::vector<sdk::metrics::Record> &records) noexcept
{
  for(auto record : records)
  {
    sout_ << "{"
          << "\n  name        : " << record.GetName()
          << "\n  description : " << record.GetDescription()
          << "\n  labels      : " << record.GetLabels();
          GetAggType(record.GetAggregator());
    sout_ << "\n}\n";
     
  }
  return sdkmetrics::ExportResult::kSuccess;
}

} // namespace metrics
} // namespace exporter
OPENTELEMETRY_END_NAMESPACE