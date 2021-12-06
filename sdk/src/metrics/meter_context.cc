// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

MeterContext::MeterContext(std::vector<std::unique_ptr<MetricExporter>> &&exporters,
                           std::vector<std::unique_ptr<MetricReader>> &&readers,
                           std::vector<std::unique_ptr<View>> &&views,
                           opentelemetry::sdk::resource::Resource resource) noexcept
    : exporters_(std::move(exporters)),
      readers_(std::move(readers)),
      views_(std::move(views)),
      resource_{resource}
{}

const resource::Resource &MeterContext::GetResource() const noexcept
{
  return resource_;
}

void MeterContext::AddMetricExporter(std::unique_ptr<MetricExporter> exporter) noexcept
{
  exporters_.push_back(std::move(exporter));
}

void MeterContext::AddMetricReader(std::unique_ptr<MetricReader> reader) noexcept
{
  readers_.push_back(std::move(reader));
}

void MeterContext::AddView(const InstrumentSelector &instrument_selector,
                           const MeterSelector &meter_selector,
                           std::unique_ptr<View> view) noexcept
{
  views_.push_back(instrument_selector, meter_selector, std::move(view));
}

bool MeterContext::Shutdown() noexcept
{
  bool result_exporter = true;
  bool result_reader   = true;
  for (auto &exporter : exporters_)
  {
    bool status     = exporter->Shutdown();
    result_exporter = result_exporter && status;
  }
  if (!result_exporter)
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Unable to shutdown all metric exporters");
  }
  for (auto &reader : readers_)
  {
    bool status   = reader->Shutdown();
    result_reader = result_reader && status;
  }
  if (!result_reader)
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::Shutdown] Unable to shutdown all metric readers");
  }
  return result_exporter && result_reader;
}

bool MeterContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  // TODO - Implement timeout logic.
  bool result_exporter = true;
  for (auto &exporter : exporters_)
  {
    bool status     = exporter->ForceFlush(timeout);
    result_exporter = result_exporter && status;
  }
  if (!result_exporter)
  {
    OTEL_INTERNAL_LOG_WARN("[MeterContext::ForceFlush] Unable to force-flush all metric exporters");
  }
  return result_exporter;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
