/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/exporters/prometheus/prometheus_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace prometheus
{
/**
 * Constructor - binds an exposer and collector to the exporter
 * @param address: an address for an exposer that exposes
 *  an HTTP endpoint for the exporter to connect to
 */
PrometheusExporter::PrometheusExporter(std::string &address) : is_shutdown_(false)
{
  exposer_   = std::unique_ptr<::prometheus::Exposer>(new ::prometheus::Exposer{address});
  collector_ = std::shared_ptr<PrometheusCollector>(new PrometheusCollector);

  exposer_->RegisterCollectable(collector_);
}

/**
 * PrometheusExporter constructor with no parameters
 * Used for testing only
 */
PrometheusExporter::PrometheusExporter() : is_shutdown_(false)
{
  collector_ = std::unique_ptr<PrometheusCollector>(new PrometheusCollector);
}

/**
 * Exports a batch of Metric Records.
 * @param records: a collection of records to export
 * @return: returns a ReturnCode detailing a success, or type of failure
 */
sdk::common::ExportResult PrometheusExporter::Export(
    const std::vector<sdk::metrics::Record> &records) noexcept
{
  if (is_shutdown_)
  {
    return sdk::common::ExportResult::kFailure;
  }
  else if (records.empty())
  {
    return sdk::common::ExportResult::kFailureInvalidArgument;
  }
  else if (collector_->GetCollection().size() + records.size() >
           (size_t)collector_->GetMaxCollectionSize())
  {
    return sdk::common::ExportResult::kFailureFull;
  }
  else
  {
    collector_->AddMetricData(records);
    return sdk::common::ExportResult::kSuccess;
  }
}

/**
 * Shuts down the exporter and does cleanup.
 * Since Prometheus is a pull based interface,
 * we cannot serve data remaining in the intermediate
 * collection to to client an HTTP request being sent,
 * so we flush the data.
 */
void PrometheusExporter::Shutdown() noexcept
{
  is_shutdown_ = true;

  collector_->GetCollection().clear();
}

/**
 * @return: returns a shared_ptr to
 * the PrometheusCollector instance
 */
std::shared_ptr<PrometheusCollector> &PrometheusExporter::GetCollector()
{
  return collector_;
}

/**
 * @return: Gets the shutdown status of the exporter
 */
bool PrometheusExporter::IsShutdown() const
{
  return is_shutdown_;
}

}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // ENABLE_METRICS_PREVIEW
