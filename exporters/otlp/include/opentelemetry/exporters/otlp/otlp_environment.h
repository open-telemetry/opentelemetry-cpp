// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/version/version.h"

#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <unordered_set>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultUserAgent()
{
  return "OTel-OTLP-Exporter-Cpp/" OPENTELEMETRY_SDK_VERSION;
}

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcTracesEndpoint();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcMetricsEndpoint();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcLogsEndpoint();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcEndpoint()
{
  return GetOtlpDefaultGrpcTracesEndpoint();
}

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultHttpTracesEndpoint();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultHttpMetricsEndpoint();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultHttpLogsEndpoint();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultHttpEndpoint()
{
  return GetOtlpDefaultHttpTracesEndpoint();
}

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsEndpoint()
{
  return GetOtlpDefaultHttpMetricsEndpoint();
}

bool OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcTracesIsInsecure();
bool OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcMetricsIsInsecure();
bool OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultGrpcLogsIsInsecure();

// Compatibility with OTELCPP 1.8.2
inline bool OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultIsSslEnable()
{
  return (!GetOtlpDefaultGrpcTracesIsInsecure());
}

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslCertificatePath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslCertificatePath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslCertificatePath();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultSslCertificatePath()
{
  return GetOtlpDefaultTracesSslCertificatePath();
}

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslCertificateString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslCertificateString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslCertificateString();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultSslCertificateString()
{
  return GetOtlpDefaultTracesSslCertificateString();
}

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslClientKeyPath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslClientKeyPath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslClientKeyPath();

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslClientKeyString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslClientKeyString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslClientKeyString();

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslClientCertificatePath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslClientCertificatePath();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslClientCertificatePath();

std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesSslClientCertificateString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsSslClientCertificateString();
std::string OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsSslClientCertificateString();

std::chrono::system_clock::duration OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsTimeout();

// Compatibility with OTELCPP 1.8.2
inline std::chrono::system_clock::duration OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTimeout()
{
  return GetOtlpDefaultTracesTimeout();
}

struct cmp_ic
{
  bool operator()(const std::string &s1, const std::string &s2) const
  {
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(),
        [](char c1, char c2) { return ::tolower(c1) < ::tolower(c2); });
  }
};
using OtlpHeaders = std::multimap<std::string, std::string, cmp_ic>;

OtlpHeaders OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultTracesHeaders();
OtlpHeaders OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultMetricsHeaders();
OtlpHeaders OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultLogsHeaders();

// Compatibility with OTELCPP 1.8.2
inline OtlpHeaders OPENTELEMETRY_EXPORTERS_OTLP_EXPORT GetOtlpDefaultHeaders()
{
  return GetOtlpDefaultTracesHeaders();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
