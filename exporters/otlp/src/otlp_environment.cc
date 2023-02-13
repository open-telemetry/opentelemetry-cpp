// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_environment.h"

#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/version/version.h"

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk_config.h"

namespace nostd      = opentelemetry::nostd;
namespace sdk_common = opentelemetry::sdk::common;

/*
  TODO:
  - Document new variables
  - Announce deprecation in CHANGELOG
  - Activate deprecation warning
*/
/* #define WARN_DEPRECATED_ENV */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static bool GetBoolDualEnvVar(const char *signal_name, const char *generic_name, bool &value)
{
  bool exists;

  exists = sdk_common::GetBoolEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetBoolEnvironmentVariable(generic_name, value);

  return exists;
}

static bool GetDurationDualEnvVar(const char *signal_name,
                                  const char *generic_name,
                                  std::chrono::system_clock::duration &value)
{
  bool exists;

  exists = sdk_common::GetDurationEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetDurationEnvironmentVariable(generic_name, value);

  return exists;
}

static bool GetStringDualEnvVar(const char *signal_name,
                                const char *generic_name,
                                std::string &value)
{
  bool exists;

  exists = sdk_common::GetStringEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetStringEnvironmentVariable(generic_name, value);

  return exists;
}

std::string GetOtlpDefaultGrpcTracesEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4317";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultGrpcMetricsEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4317";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultGrpcLogsEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4317";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpTracesEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4318/v1/traces";

  std::string value;
  bool exists;

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    value += "/v1/traces";
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpMetricsEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4318/v1/metrics";

  std::string value;
  bool exists;

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    value += "/v1/metrics";
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpLogsEndpoint()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_ENDPOINT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
  constexpr char kDefault[]    = "http://localhost:4318/v1/logs";

  std::string value;
  bool exists;

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    value += "/v1/logs";
    return value;
  }

  return kDefault;
}

bool GetOtlpDefaultTracesIsInsecure()
{
  constexpr char kSignalEnv[]     = "OTEL_EXPORTER_OTLP_TRACES_INSECURE";
  constexpr char kGenericEnv[]    = "OTEL_EXPORTER_OTLP_INSECURE";
  constexpr char kOldSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_SSL_ENABLE";
  constexpr char kOldGenericEnv[] = "OTEL_EXPORTER_OTLP_SSL_ENABLE";

  bool insecure;
  bool ssl_enabled;
  bool exists;

  exists = GetBoolDualEnvVar(kSignalEnv, kGenericEnv, insecure);
  if (exists)
  {
    return insecure;
  }

  exists = sdk_common::GetBoolEnvironmentVariable(kOldSignalEnv, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kOldSignalEnv << "> is deprecated, use <"
                                                    << kSignalEnv << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  exists = sdk_common::GetBoolEnvironmentVariable(kOldGenericEnv, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kOldGenericEnv << "> is deprecated, use <"
                                                    << kGenericEnv << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  return false;
}

bool GetOtlpDefaultMetricsIsInsecure()
{
  constexpr char kSignalEnv[]     = "OTEL_EXPORTER_OTLP_METRICS_INSECURE";
  constexpr char kGenericEnv[]    = "OTEL_EXPORTER_OTLP_INSECURE";
  constexpr char kOldSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_SSL_ENABLE";
  constexpr char kOldGenericEnv[] = "OTEL_EXPORTER_OTLP_SSL_ENABLE";

  bool insecure;
  bool ssl_enabled;
  bool exists;

  exists = GetBoolDualEnvVar(kSignalEnv, kGenericEnv, insecure);
  if (exists)
  {
    return insecure;
  }

  exists = sdk_common::GetBoolEnvironmentVariable(kOldSignalEnv, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kOldSignalEnv << "> is deprecated, use <"
                                                    << kSignalEnv << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  exists = sdk_common::GetBoolEnvironmentVariable(kOldGenericEnv, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kOldGenericEnv << "> is deprecated, use <"
                                                    << kGenericEnv << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  return false;
}

bool GetOtlpDefaultLogsIsInsecure()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_INSECURE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_INSECURE";

  bool insecure;
  bool exists;

  exists = GetBoolDualEnvVar(kSignalEnv, kGenericEnv, insecure);
  if (exists)
  {
    return insecure;
  }

  return false;
}

std::string GetOtlpDefaultTracesSslCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientKeyPath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_KEY";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientKeyPath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_KEY";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientKeyPath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_KEY";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientKeyString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_KEY_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientKeyString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_KEY_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientKeyString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_KEY_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_KEY_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientCertificatePath()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_CERTIFICATE";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientCertificateString()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_CERTIFICATE_STRING";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists;

  exists = GetStringDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::chrono::system_clock::duration GetOtlpDefaultTracesTimeout()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_TIMEOUT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists;

  exists = GetDurationDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

std::chrono::system_clock::duration GetOtlpDefaultMetricsTimeout()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_TIMEOUT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists;

  exists = GetDurationDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

std::chrono::system_clock::duration GetOtlpDefaultLogsTimeout()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_TIMEOUT";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists;

  exists = GetDurationDualEnvVar(kSignalEnv, kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

static void DumpOtlpHeaders(OtlpHeaders &output, const char *env_var_name)
{
  std::string raw_value;
  bool exists;

  exists = sdk_common::GetStringEnvironmentVariable(env_var_name, raw_value);

  if (!exists)
  {
    return;
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
        auto range = output.equal_range(key);
        if (range.first != range.second)
        {
          output.erase(range.first, range.second);
        }
      }

      std::string value(header_value);
      output.emplace(std::make_pair(std::move(key), std::move(value)));
    }
  }
}

static OtlpHeaders GetHeaders(const char *signal_name, const char *generic_name)
{
  OtlpHeaders result;
  DumpOtlpHeaders(result, generic_name);
  DumpOtlpHeaders(result, signal_name);

  return result;
}

OtlpHeaders GetOtlpDefaultTracesHeaders()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_HEADERS";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_HEADERS";

  return GetHeaders(kSignalEnv, kGenericEnv);
}

OtlpHeaders GetOtlpDefaultMetricsHeaders()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_HEADERS";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_HEADERS";

  return GetHeaders(kSignalEnv, kGenericEnv);
}

OtlpHeaders GetOtlpDefaultLogsHeaders()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_HEADERS";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_HEADERS";

  return GetHeaders(kSignalEnv, kGenericEnv);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
