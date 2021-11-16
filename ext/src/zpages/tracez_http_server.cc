// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_http_server.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{
namespace nostd = opentelemetry::nostd;

json TracezHttpServer::GetAggregations()
{
  aggregated_data_ = data_aggregator_->GetAggregatedTracezData();
  auto counts_json = json::array();

  for (const auto &aggregation_group : aggregated_data_)
  {
    const auto &buckets            = aggregation_group.second;
    const auto &complete_ok_counts = buckets.completed_span_count_per_latency_bucket;

    auto latency_counts = json::array();
    for (unsigned int boundary = 0; boundary < kLatencyBoundaries.size(); boundary++)
    {
      latency_counts.push_back(complete_ok_counts[boundary]);
    }

    counts_json.push_back({{"name", aggregation_group.first},
                           {"error", buckets.error_span_count},
                           {"running", buckets.running_span_count},
                           {"latency", latency_counts}});
  }
  return counts_json;
}

json TracezHttpServer::GetRunningSpansJSON(const std::string &name)
{
  auto running_json = json::array();

  auto grouping = aggregated_data_.find(name);

  if (grouping != aggregated_data_.end())
  {
    const auto &running_samples = grouping->second.sample_running_spans;
    for (const auto &sample : running_samples)
    {
      running_json.push_back({
          {"spanid", std::string(reinterpret_cast<const char *>(sample.GetSpanId().Id().data()))},
          {"parentid",
           std::string(reinterpret_cast<const char *>(sample.GetParentSpanId().Id().data()))},
          {"traceid", std::string(reinterpret_cast<const char *>(sample.GetTraceId().Id().data()))},
          {"start", sample.GetStartTime().time_since_epoch().count()},
          {"attributes", GetAttributesJSON(sample)},
      });
    }
  }
  return running_json;
}

json TracezHttpServer::GetErrorSpansJSON(const std::string &name)
{
  auto error_json = json::array();

  auto grouping = aggregated_data_.find(name);

  if (grouping != aggregated_data_.end())
  {
    const auto &error_samples = grouping->second.sample_error_spans;
    for (const auto &sample : error_samples)
    {
      error_json.push_back({
          {"spanid", std::string(reinterpret_cast<const char *>(sample.GetSpanId().Id().data()))},
          {"parentid",
           std::string(reinterpret_cast<const char *>(sample.GetParentSpanId().Id().data()))},
          {"traceid", std::string(reinterpret_cast<const char *>(sample.GetTraceId().Id().data()))},
          {"start", sample.GetStartTime().time_since_epoch().count()},
          {"status", (unsigned short)sample.GetStatus()},
          {"attributes", GetAttributesJSON(sample)},
      });
    }
  }
  return error_json;
}

json TracezHttpServer::GetLatencySpansJSON(const std::string &name, int latency_range_index)
{
  auto latency_json = json::array();

  auto grouping = aggregated_data_.find(name);

  if (grouping != aggregated_data_.end())
  {
    const auto &latency_samples = grouping->second.sample_latency_spans[latency_range_index];
    for (const auto &sample : latency_samples)
    {
      latency_json.push_back({
          {"spanid", std::string(reinterpret_cast<const char *>(sample.GetSpanId().Id().data()))},
          {"parentid",
           std::string(reinterpret_cast<const char *>(sample.GetParentSpanId().Id().data()))},
          {"traceid", std::string(reinterpret_cast<const char *>(sample.GetTraceId().Id().data()))},
          {"start", sample.GetStartTime().time_since_epoch().count()},
          {"duration", sample.GetDuration().count()},
          {"attributes", GetAttributesJSON(sample)},
      });
    }
  }
  return latency_json;
}

json TracezHttpServer::GetAttributesJSON(
    const opentelemetry::ext::zpages::ThreadsafeSpanData &sample)
{
  auto attributes_json = json::object();
  for (const auto &sample_attribute : sample.GetAttributes())
  {
    auto &key = sample_attribute.first;
    auto &val = sample_attribute.second;  // OwnedAttributeValue

    /* Convert variant types to into their nonvariant form. This is done this way because
       the frontend and JSON doesn't care about type, and variant's get function only allows
       const integers or literals */

    switch (val.index())
    {
      case 0:
        attributes_json[key] = nostd::get<0>(val);
        break;
      case 1:
        attributes_json[key] = nostd::get<1>(val);
        break;
      case 2:
        attributes_json[key] = nostd::get<2>(val);
        break;
      case 3:
        attributes_json[key] = nostd::get<3>(val);
        break;
      case 4:
        attributes_json[key] = nostd::get<4>(val);
        break;
      case 5:
        attributes_json[key] = nostd::get<5>(val);
        break;
      case 6:
        attributes_json[key] = nostd::get<6>(val);
        break;
      case 7:
        attributes_json[key] = nostd::get<7>(val);
        break;
      case 8:
        attributes_json[key] = nostd::get<8>(val);
        break;
      case 9:
        attributes_json[key] = nostd::get<9>(val);
        break;
    }
  }
  return attributes_json;
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
