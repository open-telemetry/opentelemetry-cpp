// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#ifndef ENABLE_METRICS_PREVIEW
#  include <algorithm>
#  include "opentelemetry/exporters/ostream/common_utils.h"
#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk_config.h"

namespace
{
std::string timeToString(opentelemetry::common::SystemTimestamp time_stamp)
{
  std::time_t epoch_time = std::chrono::system_clock::to_time_t(time_stamp);

  struct tm *tm_ptr = nullptr;
#  if defined(_MSC_VER)
  struct tm buf_tm;
  if (!gmtime_s(&buf_tm, &epoch_time))
  {
    tm_ptr = &buf_tm;
  }
#  else
  tm_ptr = std::gmtime(&epoch_time);
#  endif

  char buf[100];
  char *date_str = nullptr;
  if (tm_ptr == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("[OStream Metric] gmtime failed for " << epoch_time);
  }
  else if (std::strftime(buf, sizeof(buf), "%c", tm_ptr) > 0)
  {
    date_str = buf;
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("[OStream Metric] strftime failed for " << epoch_time);
  }

  return std::string{date_str};
}
}  // namespace

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

template <typename Container>
inline void printVec(std::ostream &os, Container &vec)
{
  using T = typename std::decay<decltype(*vec.begin())>::type;
  os << '[';
  if (vec.size() > 1)
  {
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os, ", "));
  }
  os << ']';
}

OStreamMetricExporter::OStreamMetricExporter(std::ostream &sout) noexcept : sout_(sout) {}

sdk::common::ExportResult OStreamMetricExporter::Export(
    const sdk::metrics::ResourceMetrics &data) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OStream Metric] Exporting "
                            << data.instrumentation_info_metric_data_.size()
                            << " records(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  for (auto &record : data.instrumentation_info_metric_data_)
  {
    printInstrumentationInfoMetricData(record);
  }
  return sdk::common::ExportResult::kSuccess;
}

void OStreamMetricExporter::printInstrumentationInfoMetricData(
    const sdk::metrics::InstrumentationInfoMetrics &info_metric)
{
  // sout_ is shared
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  sout_ << "{";
  sout_ << "\n  name\t\t: " << info_metric.instrumentation_library_->GetName()
        << "\n  schema url\t: " << info_metric.instrumentation_library_->GetSchemaURL()
        << "\n  version\t: " << info_metric.instrumentation_library_->GetVersion();
  for (const auto &record : info_metric.metric_data_)
  {
    sout_ << "\n  start time\t: " << timeToString(record.start_ts)
          << "\n  end time\t: " << timeToString(record.end_ts)
          << "\n  name\t\t: " << record.instrument_descriptor.name_
          << "\n  description\t: " << record.instrument_descriptor.description_
          << "\n  unit\t\t: " << record.instrument_descriptor.unit_;

    for (const auto &pd : record.point_data_attr_)
    {
      if (!nostd::holds_alternative<sdk::metrics::DropPointData>(pd.point_data))
      {
        printPointData(pd.point_data);
        printPointAttributes(pd.attributes);
      }
    }
  }
  sout_ << "\n}\n";
}

void OStreamMetricExporter::printPointData(const opentelemetry::sdk::metrics::PointType &point_data)
{
  if (nostd::holds_alternative<sdk::metrics::SumPointData>(point_data))
  {
    auto sum_point_data = nostd::get<sdk::metrics::SumPointData>(point_data);
    sout_ << "\n  type\t\t: SumPointData";
    sout_ << "\n  value\t\t: ";
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
    if (nostd::holds_alternative<std::list<double>>(histogram_point_data.boundaries_))
    {
      auto &double_boundaries = nostd::get<std::list<double>>(histogram_point_data.boundaries_);
      printVec(sout_, double_boundaries);
    }
    else if (nostd::holds_alternative<std::list<long>>(histogram_point_data.boundaries_))
    {
      auto &long_boundaries = nostd::get<std::list<long>>(histogram_point_data.boundaries_);
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
          << std::to_string(last_point_data.sample_ts_.time_since_epoch().count()) << std::boolalpha
          << "\n  valid     : " << last_point_data.is_lastvalue_valid_;
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
}

void OStreamMetricExporter::printPointAttributes(
    const opentelemetry::sdk::metrics::PointAttributes &point_attributes)
{
  sout_ << "\n  attributes\t\t: ";
  for (const auto &kv : point_attributes)
  {
    sout_ << "\n\t" << kv.first << ": ";
    opentelemetry::exporter::ostream_common::print_value(kv.second, sout_);
  }
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
