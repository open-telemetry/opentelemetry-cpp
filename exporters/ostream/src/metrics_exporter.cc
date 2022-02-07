// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/exporters/ostream/metrics_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

OStreamMetricsExporter::OStreamMetricsExporter(std::ostream &sout) noexcept : sout_(sout) {}

sdk::common::ExportResult OStreamMetricsExporter::Export(
    const std::vector<sdk::metrics::Record> &records) noexcept
{
  for (auto record : records)
  {
    sout_ << "{"
          << "\n  name        : " << record.GetName()
          << "\n  description : " << record.GetDescription()
          << "\n  labels      : " << record.GetLabels();

    auto aggregator = record.GetAggregator();

    /**
     * Unpack the AggregatorVariant from the record so we can pass the data type to
     * PrintAggregatorVariant to unpack the Aggregator from the variant.
     */
    if (nostd::holds_alternative<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(
            aggregator))
    {
      PrintAggregatorVariant<int>(aggregator);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(aggregator))
    {
      PrintAggregatorVariant<short>(aggregator);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator))
    {
      PrintAggregatorVariant<double>(aggregator);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator))
    {
      PrintAggregatorVariant<float>(aggregator);
    }
    sout_ << "\n}\n";
  }
  return sdk::common::ExportResult::kSuccess;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
