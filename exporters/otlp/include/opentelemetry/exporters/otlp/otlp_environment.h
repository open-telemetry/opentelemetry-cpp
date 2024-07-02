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

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcTracesEndpoint();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcMetricsEndpoint();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcLogsEndpoint();

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultGrpcEndpoint()
{
  return GetOtlpDefaultGrpcTracesEndpoint();
}

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultHttpTracesEndpoint();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultHttpMetricsEndpoint();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultHttpLogsEndpoint();

std::string GetOtlpDefaultHttpTracesProtocol();
std::string GetOtlpDefaultHttpMetricsProtocol();
std::string GetOtlpDefaultHttpLogsProtocol();

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultHttpEndpoint()
{
  return GetOtlpDefaultHttpTracesEndpoint();
}

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultMetricsEndpoint()
{
  return GetOtlpDefaultHttpMetricsEndpoint();
}

bool OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcTracesIsInsecure();
bool OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcMetricsIsInsecure();
bool OPENTELEMETRY_EXPORT GetOtlpDefaultGrpcLogsIsInsecure();

// Compatibility with OTELCPP 1.8.2
inline bool GetOtlpDefaultIsSslEnable()
{
  return (!GetOtlpDefaultGrpcTracesIsInsecure());
}

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslCertificatePath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslCertificatePath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslCertificatePath();

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultSslCertificatePath()
{
  return GetOtlpDefaultTracesSslCertificatePath();
}

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslCertificateString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslCertificateString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslCertificateString();

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultSslCertificateString()
{
  return GetOtlpDefaultTracesSslCertificateString();
}

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslClientKeyPath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslClientKeyPath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslClientKeyPath();

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslClientKeyString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslClientKeyString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslClientKeyString();

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslClientCertificatePath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslClientCertificatePath();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslClientCertificatePath();

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslClientCertificateString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslClientCertificateString();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslClientCertificateString();

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslTlsMinVersion();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslTlsMinVersion();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslTlsMinVersion();

std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslTlsMaxVersion();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslTlsMaxVersion();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslTlsMaxVersion();

// For TLS 1.2
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslTlsCipher();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslTlsCipher();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslTlsCipher();

// For TLS 1.3
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultTracesSslTlsCipherSuite();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsSslTlsCipherSuite();
std::string OPENTELEMETRY_EXPORT GetOtlpDefaultLogsSslTlsCipherSuite();

std::chrono::system_clock::duration OPENTELEMETRY_EXPORT GetOtlpDefaultTracesTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsTimeout();
std::chrono::system_clock::duration OPENTELEMETRY_EXPORT GetOtlpDefaultLogsTimeout();

// Compatibility with OTELCPP 1.8.2
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

OtlpHeaders OPENTELEMETRY_EXPORT GetOtlpDefaultTracesHeaders();
OtlpHeaders OPENTELEMETRY_EXPORT GetOtlpDefaultMetricsHeaders();
OtlpHeaders OPENTELEMETRY_EXPORT GetOtlpDefaultLogsHeaders();

// Compatibility with OTELCPP 1.8.2
inline OtlpHeaders GetOtlpDefaultHeaders()
{
  return GetOtlpDefaultTracesHeaders();
}

std::string GetOtlpDefaultTracesCompression();
std::string GetOtlpDefaultMetricsCompression();
std::string GetOtlpDefaultLogsCompression();

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
