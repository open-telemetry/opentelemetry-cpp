// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      PrintAggregatorVariant<int>(aggregator);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      PrintAggregatorVariant<short>(aggregator);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      PrintAggregatorVariant<double>(aggregator);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
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
