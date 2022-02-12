// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include <algorithm>
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

OStreamMetricExporter::OStreamMetricExporter(std::ostream &sout) noexcept : sout_(sout) {}

sdk::common::ExportResult OStreamMetricExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::metrics::MetricData>> &records) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OStream Metric] Exporting "
                            << records.size() << " records(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  for (auto &record : records)
  {
    sout_ << "{"
          << "\n  name        : " << record->instrumentation_library_->GetName()
          << "\n  version     : " << record->instrumentation_library_->GetVersion();
    printPointData(record->point_data_);
    sout_ << "\n}\n";
  }
  return sdk::common::ExportResult::kSuccess;
}

template <typename T>
inline void printVec(std::ostream &os, std::vector<T> &vec)
{
  os << '[';
  if (vec.size() > 1)
  {
    std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>(os, ", "));
  }
  if (!vec.empty())
  {
    os << vec.back();
  }
  os << ']';
}

void OStreamMetricExporter::printPointData(opentelemetry::sdk::metrics::PointType &point_data)
{
  if (nostd::holds_alternative<sdk::metrics::SumPointData>(point_data))
  {
    auto sum_point_data = nostd::get<sdk::metrics::SumPointData>(point_data);
    sout_ << "\n  type     : SumPointData";
    sout_ << "\n  start timestamp     : "
          << std::to_string(sum_point_data.start_epoch_nanos_.time_since_epoch().count());
    sout_ << "\n  end timestamp     : "
          << std::to_string(sum_point_data.end_epoch_nanos_.time_since_epoch().count());
    sout_ << "\n  value     : ";
    if (nostd::holds_alternative<double>(sum_point_data.value_))
    {
      sout_ << nostd::get<double>(sum_point_data.value_);
    }
    else if (nostd::holds_alternative<long>(sum_point_data.value_))
    {
      sout_ << nostd::get<long>(sum_point_data.value_);
    }
  }
  else if (nostd::holds_alternative<sdk::metrics::HistogramPointData>(point_data))
  {
    auto histogram_point_data = nostd::get<sdk::metrics::HistogramPointData>(point_data);
    sout_ << "\n  type     : HistogramPointData";
    sout_ << "\n  timestamp     : "
          << std::to_string(histogram_point_data.epoch_nanos_.time_since_epoch().count());
    sout_ << "\n  count     : " << histogram_point_data.count_;
    sout_ << "\n  sum     : ";
    if (nostd::holds_alternative<double>(histogram_point_data.sum_))
    {
      sout_ << nostd::get<double>(histogram_point_data.sum_);
    }
    else if (nostd::holds_alternative<long>(histogram_point_data.sum_))
    {
      sout_ << nostd::get<long>(histogram_point_data.sum_);
    }

    sout_ << "\n  buckets     : ";
    if (nostd::holds_alternative<std::vector<double>>(histogram_point_data.boundaries_))
    {
      auto &double_boundaries = nostd::get<std::vector<double>>(histogram_point_data.boundaries_);
      printVec(sout_, double_boundaries);
    }
    else if (nostd::holds_alternative<std::vector<long>>(histogram_point_data.boundaries_))
    {
      auto &long_boundaries = nostd::get<std::vector<long>>(histogram_point_data.boundaries_);
      printVec(sout_, long_boundaries);
    }

    sout_ << "\n  counts     : ";
    printVec(sout_, histogram_point_data.counts_);
  }
  else if (nostd::holds_alternative<sdk::metrics::LastValuePointData>(point_data))
  {
    auto last_point_data = nostd::get<sdk::metrics::LastValuePointData>(point_data);
    sout_ << "\n  type     : LastValuePointData";
    sout_ << "\n  timestamp     : "
          << std::to_string(last_point_data.epoch_nanos_.time_since_epoch().count())
          << std::boolalpha << "\n  valid     : " << last_point_data.is_lastvalue_valid_;
    sout_ << "\n  value     : ";
    if (nostd::holds_alternative<double>(last_point_data.value_))
    {
      sout_ << nostd::get<double>(last_point_data.value_);
    }
    else if (nostd::holds_alternative<long>(last_point_data.value_))
    {
      sout_ << nostd::get<long>(last_point_data.value_);
    }
  }
  else if (nostd::holds_alternative<sdk::metrics::DropPointData>(point_data))
  {}
}

bool OStreamMetricExporter::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return true;
}

bool OStreamMetricExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OStreamMetricExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
