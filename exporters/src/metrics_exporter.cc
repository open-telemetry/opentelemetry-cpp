#include "opentelemetry/exporters/stream/metrics_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{
StreamMetricsExporter::StreamMetricsExporter(std::ostream &sout) noexcept
                                    : sout_{sout} {}

sdkmetrics::ExportResult StreamMetricsExporter::Export(
  const std::vector<sdkmetrics::Record> &records) noexcept
{


  return sdkmetrics::ExportResult::kSuccess;
}

void StreamMetricsExporter::Shutdown(std::chrono::microseconds timeout) noexcept 
{
  isShutdown_ = true;
}

} // namespace trace
} // namespace exporter
OPENTELEMETRY_END_NAMESPACE