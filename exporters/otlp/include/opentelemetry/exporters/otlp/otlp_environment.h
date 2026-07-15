// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <ctype.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/version.h"

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

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultGrpcEndpoint()
{
  return GetOtlpDefaultGrpcTracesEndpoint();
}

std::string GetOtlpDefaultHttpTracesEndpoint();
std::string GetOtlpDefaultHttpMetricsEndpoint();
std::string GetOtlpDefaultHttpLogsEndpoint();

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

bool GetOtlpDefaultGrpcTracesIsInsecure();
bool GetOtlpDefaultGrpcMetricsIsInsecure();
bool GetOtlpDefaultGrpcLogsIsInsecure();

// Compatibility with OTELCPP 1.8.2
inline bool GetOtlpDefaultIsSslEnable()
{
  return (!GetOtlpDefaultGrpcTracesIsInsecure());
}

std::string GetOtlpDefaultTracesSslCertificatePath();
std::string GetOtlpDefaultMetricsSslCertificatePath();
std::string GetOtlpDefaultLogsSslCertificatePath();

// Compatibility with OTELCPP 1.8.2
inline std::string GetOtlpDefaultSslCertificatePath()
{
  return GetOtlpDefaultTracesSslCertificatePath();
}

std::string GetOtlpDefaultTracesSslCertificateString();
std::string GetOtlpDefaultMetricsSslCertificateString();
std::string GetOtlpDefaultLogsSslCertificateString();

// Compatibility with OTELCPP 1.8.2
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

std::string GetOtlpDefaultTracesSslTlsMinVersion();
std::string GetOtlpDefaultMetricsSslTlsMinVersion();
std::string GetOtlpDefaultLogsSslTlsMinVersion();

std::string GetOtlpDefaultTracesSslTlsMaxVersion();
std::string GetOtlpDefaultMetricsSslTlsMaxVersion();
std::string GetOtlpDefaultLogsSslTlsMaxVersion();

// For TLS 1.2
std::string GetOtlpDefaultTracesSslTlsCipher();
std::string GetOtlpDefaultMetricsSslTlsCipher();
std::string GetOtlpDefaultLogsSslTlsCipher();

// For TLS 1.3
std::string GetOtlpDefaultTracesSslTlsCipherSuite();
std::string GetOtlpDefaultMetricsSslTlsCipherSuite();
std::string GetOtlpDefaultLogsSslTlsCipherSuite();

std::chrono::system_clock::duration GetOtlpDefaultTracesTimeout();
std::chrono::system_clock::duration GetOtlpDefaultMetricsTimeout();
std::chrono::system_clock::duration GetOtlpDefaultLogsTimeout();

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

OtlpHeaders GetOtlpDefaultTracesHeaders();
OtlpHeaders GetOtlpDefaultMetricsHeaders();
OtlpHeaders GetOtlpDefaultLogsHeaders();

// Compatibility with OTELCPP 1.8.2
inline OtlpHeaders GetOtlpDefaultHeaders()
{
  return GetOtlpDefaultTracesHeaders();
}

std::string GetOtlpDefaultTracesCompression();
std::string GetOtlpDefaultMetricsCompression();
std::string GetOtlpDefaultLogsCompression();

std::uint32_t GetOtlpDefaultTracesRetryMaxAttempts();
std::uint32_t GetOtlpDefaultMetricsRetryMaxAttempts();
std::uint32_t GetOtlpDefaultLogsRetryMaxAttempts();

std::chrono::duration<float> GetOtlpDefaultTracesRetryInitialBackoff();
std::chrono::duration<float> GetOtlpDefaultMetricsRetryInitialBackoff();
std::chrono::duration<float> GetOtlpDefaultLogsRetryInitialBackoff();

std::chrono::duration<float> GetOtlpDefaultTracesRetryMaxBackoff();
std::chrono::duration<float> GetOtlpDefaultMetricsRetryMaxBackoff();
std::chrono::duration<float> GetOtlpDefaultLogsRetryMaxBackoff();

float GetOtlpDefaultTracesRetryBackoffMultiplier();
float GetOtlpDefaultMetricsRetryBackoffMultiplier();
float GetOtlpDefaultLogsRetryBackoffMultiplier();

/**
 * Signal-independent accessors, for use by a gRPC client shared across
 * multiple signal exporters. These read only the generic OTEL_EXPORTER_OTLP_*
 * environment variables and fall back to the same spec defaults used above.
 */

inline std::string GetOtlpDefaultGrpcClientEndpoint()
{
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4317";

  std::string value;

  if (opentelemetry::sdk::common::GetStringEnvironmentVariable(kGenericEnv, value))
  {
    return value;
  }

  return kDefault;
}

inline bool GetOtlpDefaultGrpcClientIsInsecure()
{
  std::string endpoint = GetOtlpDefaultGrpcClientEndpoint();

  if (endpoint.substr(0, 6) == "https:")
  {
    return false;
  }

  if (endpoint.substr(0, 5) == "http:")
  {
    return true;
  }

  constexpr char kGenericEnv[]    = "OTEL_EXPORTER_OTLP_INSECURE";
  constexpr char kOldGenericEnv[] = "OTEL_EXPORTER_OTLP_SSL_ENABLE";

  bool insecure{};
  bool ssl_enabled{};

  if (opentelemetry::sdk::common::GetBoolEnvironmentVariable(kGenericEnv, insecure))
  {
    return insecure;
  }

  if (opentelemetry::sdk::common::GetBoolEnvironmentVariable(kOldGenericEnv, ssl_enabled))
  {
    return !ssl_enabled;
  }

  return false;
}

inline std::string GetOtlpDefaultGrpcClientSslCertificatePath()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_CERTIFICATE", value);
  return value;
}

inline std::string GetOtlpDefaultGrpcClientSslCertificateString()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_CERTIFICATE_STRING",
                                                           value);
  return value;
}

inline std::string GetOtlpDefaultGrpcClientSslClientKeyPath()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_CLIENT_KEY", value);
  return value;
}

inline std::string GetOtlpDefaultGrpcClientSslClientKeyString()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_CLIENT_KEY_STRING",
                                                           value);
  return value;
}

inline std::string GetOtlpDefaultGrpcClientSslClientCertificatePath()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE",
                                                           value);
  return value;
}

inline std::string GetOtlpDefaultGrpcClientSslClientCertificateString()
{
  std::string value;
  opentelemetry::sdk::common::GetStringEnvironmentVariable(
      "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE_STRING", value);
  return value;
}

inline std::chrono::system_clock::duration GetOtlpDefaultGrpcClientTimeout()
{
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_TIMEOUT";

  std::chrono::system_clock::duration value;

  if (opentelemetry::sdk::common::GetDurationEnvironmentVariable(kGenericEnv, value))
  {
    return value;
  }

  return std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
}

inline OtlpHeaders GetOtlpDefaultGrpcClientHeaders()
{
  OtlpHeaders result;

  std::string raw_value;
  if (!opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_HEADERS",
                                                                raw_value))
  {
    return result;
  }

  opentelemetry::common::KeyValueStringTokenizer tokenizer{raw_value};
  opentelemetry::nostd::string_view header_key;
  opentelemetry::nostd::string_view header_value;
  bool header_valid = true;

  std::unordered_set<std::string> remove_cache;

  while (tokenizer.next(header_valid, header_key, header_value))
  {
    if (header_valid)
    {
      std::string key(header_key);
      if (remove_cache.end() == remove_cache.find(key))
      {
        remove_cache.insert(key);
        auto range = result.equal_range(key);
        if (range.first != range.second)
        {
          result.erase(range.first, range.second);
        }
      }

      std::string value(header_value);
      result.emplace(std::make_pair(std::move(key), std::move(value)));
    }
  }

  return result;
}

inline std::string GetOtlpDefaultGrpcClientCompression()
{
  std::string value;

  if (opentelemetry::sdk::common::GetStringEnvironmentVariable("OTEL_EXPORTER_OTLP_COMPRESSION",
                                                               value))
  {
    return value;
  }

  return std::string{"none"};
}

inline std::uint32_t GetOtlpDefaultGrpcClientRetryMaxAttempts()
{
  std::uint32_t value{};

  if (opentelemetry::sdk::common::GetUintEnvironmentVariable(
          "OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS", value))
  {
    return value;
  }

  return 5U;
}

inline std::chrono::duration<float> GetOtlpDefaultGrpcClientRetryInitialBackoff()
{
  float value{};

  if (opentelemetry::sdk::common::GetFloatEnvironmentVariable(
          "OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF", value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{1.0f};
}

inline std::chrono::duration<float> GetOtlpDefaultGrpcClientRetryMaxBackoff()
{
  float value{};

  if (opentelemetry::sdk::common::GetFloatEnvironmentVariable(
          "OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF", value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{5.0f};
}

inline float GetOtlpDefaultGrpcClientRetryBackoffMultiplier()
{
  float value{};

  if (opentelemetry::sdk::common::GetFloatEnvironmentVariable(
          "OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER", value))
  {
    return value;
  }

  return 1.5f;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
