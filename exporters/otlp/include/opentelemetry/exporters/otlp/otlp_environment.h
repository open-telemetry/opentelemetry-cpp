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

inline std::string GetOtlpDefaultUserAgent()
{
  return "OTel-OTLP-Exporter-Cpp/" OPENTELEMETRY_SDK_VERSION;
}

std::string GetOtlpDefaultGrpcTracesEndpoint();
std::string GetOtlpDefaultGrpcMetricsEndpoint();
std::string GetOtlpDefaultGrpcLogsEndpoint();

// Compatibility with OTELCPP 1.7.0
inline std::string GetOtlpDefaultGrpcEndpoint()
{
  return GetOtlpDefaultGrpcTracesEndpoint();
}

std::string GetOtlpDefaultHttpTracesEndpoint();
std::string GetOtlpDefaultHttpMetricsEndpoint();
std::string GetOtlpDefaultHttpLogsEndpoint();

// Compatibility with OTELCPP 1.7.0
inline std::string GetOtlpDefaultHttpEndpoint()
{
  return GetOtlpDefaultHttpTracesEndpoint();
}

// Compatibility with OTELCPP 1.7.0
inline std::string GetOtlpDefaultMetricsEndpoint()
{
  return GetOtlpDefaultHttpMetricsEndpoint();
}


bool GetOtlpDefaultTracesIsInsecure();
bool GetOtlpDefaultMetricsIsInsecure();
bool GetOtlpDefaultLogsIsInsecure();

// Compatibility with OTELCPP 1.7.0
inline bool GetOtlpDefaultIsSslEnable()
{
  return (!GetOtlpDefaultTracesIsInsecure());
}

std::string GetOtlpDefaultTracesSslCertificatePath();
std::string GetOtlpDefaultMetricsSslCertificatePath();
std::string GetOtlpDefaultLogsSslCertificatePath();

// Compatibility with OTELCPP 1.7.0
inline std::string GetOtlpDefaultSslCertificatePath()
{
  return GetOtlpDefaultTracesSslCertificatePath();
}

std::string GetOtlpDefaultTracesSslCertificateString();
std::string GetOtlpDefaultMetricsSslCertificateString();
std::string GetOtlpDefaultLogsSslCertificateString();

// Compatibility with OTELCPP 1.7.0
inline std::string GetOtlpDefaultSslCertificateString()
{
  return GetOtlpDefaultTracesSslCertificateString();
}

std::string GetOtlpDefaultTracesSslClientKeyPath();
std::string GetOtlpDefaultMetricsSslClientKeyPath();
std::string GetOtlpDefaultLogsSslClientKeyPath();

std::string GetOtlpDefaultTracesSslClientKeyString();
std::string GetOtlpDefaultMetricsSslClientKeyString();
std::string GetOtlpDefaultLogsSslClientKeyString();

std::string GetOtlpDefaultTracesSslClientCertificatePath();
std::string GetOtlpDefaultMetricsSslClientCertificatePath();
std::string GetOtlpDefaultLogsSslClientCertificatePath();

std::string GetOtlpDefaultTracesSslClientCertificateString();
std::string GetOtlpDefaultMetricsSslClientCertificateString();
std::string GetOtlpDefaultLogsSslClientCertificateString();

std::chrono::system_clock::duration GetOtlpDefaultTracesTimeout();
std::chrono::system_clock::duration GetOtlpDefaultMetricsTimeout();
std::chrono::system_clock::duration GetOtlpDefaultLogsTimeout();

// Compatibility with OTELCPP 1.7.0
inline std::chrono::system_clock::duration GetOtlpDefaultTimeout()
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

OtlpHeaders GetOtlpDefaultTracesHeaders();
OtlpHeaders GetOtlpDefaultMetricsHeaders();
OtlpHeaders GetOtlpDefaultLogsHeaders();

// Compatibility with OTELCPP 1.7.0
inline OtlpHeaders GetOtlpDefaultHeaders()
{
  return GetOtlpDefaultTracesHeaders();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
