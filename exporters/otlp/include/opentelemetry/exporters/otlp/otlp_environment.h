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

inline std::string OPENTELEMETRY_API GetOtlpDefaultUserAgent()
{
  return "OTel-OTLP-Exporter-Cpp/" OPENTELEMETRY_SDK_VERSION;
}

std::string OPENTELEMETRY_API GetOtlpDefaultGrpcTracesEndpoint();
std::string OPENTELEMETRY_API GetOtlpDefaultGrpcMetricsEndpoint();
std::string OPENTELEMETRY_API GetOtlpDefaultGrpcLogsEndpoint();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_API GetOtlpDefaultGrpcEndpoint()
{
  return GetOtlpDefaultGrpcTracesEndpoint();
}

std::string OPENTELEMETRY_API GetOtlpDefaultHttpTracesEndpoint();
std::string OPENTELEMETRY_API GetOtlpDefaultHttpMetricsEndpoint();
std::string OPENTELEMETRY_API GetOtlpDefaultHttpLogsEndpoint();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_API GetOtlpDefaultHttpEndpoint()
{
  return GetOtlpDefaultHttpTracesEndpoint();
}

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_API GetOtlpDefaultMetricsEndpoint()
{
  return GetOtlpDefaultHttpMetricsEndpoint();
}

bool OPENTELEMETRY_API GetOtlpDefaultGrpcTracesIsInsecure();
bool OPENTELEMETRY_API GetOtlpDefaultGrpcMetricsIsInsecure();
bool OPENTELEMETRY_API GetOtlpDefaultGrpcLogsIsInsecure();

// Compatibility with OTELCPP 1.8.2
inline bool OPENTELEMETRY_API GetOtlpDefaultIsSslEnable()
{
  return (!GetOtlpDefaultGrpcTracesIsInsecure());
}

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslCertificatePath();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslCertificatePath();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslCertificatePath();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_API GetOtlpDefaultSslCertificatePath()
{
  return GetOtlpDefaultTracesSslCertificatePath();
}

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslCertificateString();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslCertificateString();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslCertificateString();

// Compatibility with OTELCPP 1.8.2
inline std::string OPENTELEMETRY_API GetOtlpDefaultSslCertificateString()
{
  return GetOtlpDefaultTracesSslCertificateString();
}

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslClientKeyPath();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslClientKeyPath();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslClientKeyPath();

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslClientKeyString();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslClientKeyString();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslClientKeyString();

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslClientCertificatePath();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslClientCertificatePath();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslClientCertificatePath();

std::string OPENTELEMETRY_API GetOtlpDefaultTracesSslClientCertificateString();
std::string OPENTELEMETRY_API GetOtlpDefaultMetricsSslClientCertificateString();
std::string OPENTELEMETRY_API GetOtlpDefaultLogsSslClientCertificateString();

std::chrono::system_clock::duration OPENTELEMETRY_API GetOtlpDefaultTracesTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_API GetOtlpDefaultMetricsTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_API GetOtlpDefaultLogsTimeout();

// Compatibility with OTELCPP 1.8.2
inline std::chrono::system_clock::duration OPENTELEMETRY_API GetOtlpDefaultTimeout()
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

OtlpHeaders OPENTELEMETRY_API GetOtlpDefaultTracesHeaders();
OtlpHeaders OPENTELEMETRY_API GetOtlpDefaultMetricsHeaders();
OtlpHeaders OPENTELEMETRY_API GetOtlpDefaultLogsHeaders();

// Compatibility with OTELCPP 1.8.2
inline OtlpHeaders OPENTELEMETRY_API GetOtlpDefaultHeaders()
{
  return GetOtlpDefaultTracesHeaders();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
