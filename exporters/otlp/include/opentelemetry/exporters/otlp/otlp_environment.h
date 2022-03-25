// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/env_variables.h"

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

inline const std::string GetOtlpDefaultGrpcEndpoint()
{
  constexpr char kOtlpTracesEndpointEnv[] = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kOtlpEndpointEnv[]       = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kOtlpEndpointDefault[]   = "http://localhost:4317";

  auto endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesEndpointEnv);
  if (endpoint.empty())
  {
    endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpEndpointEnv);
  }
  return endpoint.size() ? endpoint : kOtlpEndpointDefault;
}

inline const std::string GetOtlpDefaultHttpEndpoint()
{
  constexpr char kOtlpTracesEndpointEnv[] = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kOtlpEndpointEnv[]       = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kOtlpEndpointDefault[]   = "http://localhost:4318/v1/traces";

  auto endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesEndpointEnv);
  if (endpoint.empty())
  {
    endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpEndpointEnv);
    if (!endpoint.empty())
    {
      endpoint += "/v1/traces";
    }
  }
  return endpoint.size() ? endpoint : kOtlpEndpointDefault;
}

inline bool GetOtlpDefaultIsSslEnable()
{
  constexpr char kOtlpTracesIsSslEnableEnv[] = "OTEL_EXPORTER_OTLP_TRACES_SSL_ENABLE";
  constexpr char kOtlpIsSslEnableEnv[]       = "OTEL_EXPORTER_OTLP_SSL_ENABLE";

  auto ssl_enable = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesIsSslEnableEnv);
  if (ssl_enable.empty())
  {
    ssl_enable = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpIsSslEnableEnv);
  }
  if (ssl_enable == "True" || ssl_enable == "TRUE" || ssl_enable == "true" || ssl_enable == "1")
  {
    return true;
  }
  return false;
}

inline const std::string GetOtlpDefaultSslCertificatePath()
{
  constexpr char kOtlpTracesSslCertificate[] = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE";
  constexpr char kOtlpSslCertificate[]       = "OTEL_EXPORTER_OTLP_CERTIFICATE ";
  auto ssl_cert_path =
      opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesSslCertificate);
  if (ssl_cert_path.empty())
  {
    ssl_cert_path = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpSslCertificate);
  }
  return ssl_cert_path.size() ? ssl_cert_path : "";
}

inline const std::string GetOtlpDefaultSslCertificateString()
{
  constexpr char kOtlpTracesSslCertificateString[] = "OTEL_EXPORTER_OTLP_CERTIFICATE_STRING";
  constexpr char kOtlpSslCertificateString[] = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE_STRING ";
  auto ssl_cert =
      opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesSslCertificateString);
  if (ssl_cert.empty())
  {
    ssl_cert = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpSslCertificateString);
  }
  return ssl_cert.size() ? ssl_cert : "";
}

inline const std::chrono::system_clock::duration GetOtlpTimeoutFromString(const char *input)
{
  if (nullptr == input || 0 == *input)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::seconds{10});
  }

  std::chrono::system_clock::duration::rep result = 0;
  // Skip spaces
  for (; *input && (' ' == *input || '\t' == *input || '\r' == *input || '\n' == *input); ++input)
    ;

  for (; *input && (*input >= '0' && *input <= '9'); ++input)
  {
    result = result * 10 + (*input - '0');
  }

  opentelemetry::nostd::string_view unit{input};
  if ("ns" == unit)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::nanoseconds{result});
  }
  else if ("us" == unit)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::microseconds{result});
  }
  else if ("ms" == unit)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::milliseconds{result});
  }
  else if ("m" == unit)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::minutes{result});
  }
  else if ("h" == unit)
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::hours{result});
  }
  else
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::seconds{result});
  }
}

inline const std::chrono::system_clock::duration GetOtlpDefaultTimeout()
{
  constexpr char kOtlpTracesTimeoutEnv[] = "OTEL_EXPORTER_OTLP_TRACES_TIMEOUT";
  constexpr char kOtlpTimeoutEnv[]       = "OTEL_EXPORTER_OTLP_TIMEOUT";

  auto timeout = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTracesTimeoutEnv);
  if (timeout.empty())
  {
    timeout = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTimeoutEnv);
  }
  return GetOtlpTimeoutFromString(timeout.c_str());
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

inline void DumpOtlpHeaders(OtlpHeaders &output,
                            const char *env_var_name,
                            std::unordered_set<std::string> &remove_cache)
{
  auto value = opentelemetry::sdk::common::GetEnvironmentVariable(env_var_name);
  if (value.empty())
  {
    return;
  }

  opentelemetry::common::KeyValueStringTokenizer tokenizer{value};
  opentelemetry::nostd::string_view header_key;
  opentelemetry::nostd::string_view header_value;
  bool header_valid = true;

  while (tokenizer.next(header_valid, header_key, header_value))
  {
    if (header_valid)
    {
      std::string key = static_cast<std::string>(header_key);
      if (remove_cache.end() == remove_cache.find(key))
      {
        remove_cache.insert(key);
        auto range = output.equal_range(key);
        if (range.first != range.second)
        {
          output.erase(range.first, range.second);
        }
      }

      output.emplace(std::make_pair(std::move(key), static_cast<std::string>(header_value)));
    }
  }
}

inline OtlpHeaders GetOtlpDefaultHeaders()
{
  constexpr char kOtlpTracesHeadersEnv[] = "OTEL_EXPORTER_OTLP_TRACES_HEADERS";
  constexpr char kOtlpHeadersEnv[]       = "OTEL_EXPORTER_OTLP_HEADERS";

  OtlpHeaders result;
  std::unordered_set<std::string> trace_remove_cache;
  DumpOtlpHeaders(result, kOtlpHeadersEnv, trace_remove_cache);

  trace_remove_cache.clear();
  DumpOtlpHeaders(result, kOtlpTracesHeadersEnv, trace_remove_cache);

  return result;
}

inline const std::string GetOtlpDefaultHttpLogEndpoint()
{
  constexpr char kOtlpLogsEndpointEnv[] = "OTEL_EXPORTER_OTLP_LOGS_ENDPOINT";
  constexpr char kOtlpEndpointEnv[]     = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kOtlpEndpointDefault[] = "http://localhost:4318/v1/logs";

  auto endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpLogsEndpointEnv);
  if (endpoint.empty())
  {
    endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpEndpointEnv);
    if (!endpoint.empty())
    {
      endpoint += "/v1/logs";
    }
  }
  return endpoint.size() ? endpoint : kOtlpEndpointDefault;
}

inline const std::chrono::system_clock::duration GetOtlpDefaultLogTimeout()
{
  constexpr char kOtlpLogsTimeoutEnv[] = "OTEL_EXPORTER_OTLP_LOGS_TIMEOUT";
  constexpr char kOtlpTimeoutEnv[]     = "OTEL_EXPORTER_OTLP_TIMEOUT";

  auto timeout = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpLogsTimeoutEnv);
  if (timeout.empty())
  {
    timeout = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpTimeoutEnv);
  }
  return GetOtlpTimeoutFromString(timeout.c_str());
}

inline OtlpHeaders GetOtlpDefaultLogHeaders()
{
  constexpr char kOtlpLogsHeadersEnv[] = "OTEL_EXPORTER_OTLP_LOGS_HEADERS";
  constexpr char kOtlpHeadersEnv[]     = "OTEL_EXPORTER_OTLP_HEADERS";

  OtlpHeaders result;
  std::unordered_set<std::string> log_remove_cache;
  DumpOtlpHeaders(result, kOtlpHeadersEnv, log_remove_cache);

  log_remove_cache.clear();
  DumpOtlpHeaders(result, kOtlpLogsHeadersEnv, log_remove_cache);

  return result;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
