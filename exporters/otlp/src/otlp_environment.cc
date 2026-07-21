// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/exporters/otlp/otlp_environment.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/version.h"

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

static constexpr char kEnvOtlpEndpoint[]          = "OTEL_EXPORTER_OTLP_ENDPOINT";
static constexpr char kEnvOtlpInsecure[]          = "OTEL_EXPORTER_OTLP_INSECURE";
static constexpr char kEnvOtlpSslEnable[]         = "OTEL_EXPORTER_OTLP_SSL_ENABLE";
static constexpr char kEnvOtlpCertificate[]       = "OTEL_EXPORTER_OTLP_CERTIFICATE";
static constexpr char kEnvOtlpCertificateString[] = "OTEL_EXPORTER_OTLP_CERTIFICATE_STRING";
static constexpr char kEnvOtlpClientKey[]         = "OTEL_EXPORTER_OTLP_CLIENT_KEY";
static constexpr char kEnvOtlpClientKeyString[]   = "OTEL_EXPORTER_OTLP_CLIENT_KEY_STRING";
static constexpr char kEnvOtlpClientCertificate[] = "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE";
static constexpr char kEnvOtlpClientCertificateString[] =
    "OTEL_EXPORTER_OTLP_CLIENT_CERTIFICATE_STRING";
static constexpr char kEnvOtlpTimeout[]          = "OTEL_EXPORTER_OTLP_TIMEOUT";
static constexpr char kEnvOtlpHeaders[]          = "OTEL_EXPORTER_OTLP_HEADERS";
static constexpr char kEnvOtlpCompression[]      = "OTEL_EXPORTER_OTLP_COMPRESSION";
static constexpr char kEnvOtlpRetryMaxAttempts[] = "OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS";
static constexpr char kEnvOtlpRetryInitialBackoff[] =
    "OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF";
static constexpr char kEnvOtlpRetryMaxBackoff[] = "OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF";
static constexpr char kEnvOtlpRetryBackoffMultiplier[] =
    "OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER";
static constexpr char kEnvOtlpMinTls[]      = "OTEL_CPP_EXPORTER_OTLP_MIN_TLS";
static constexpr char kEnvOtlpMaxTls[]      = "OTEL_CPP_EXPORTER_OTLP_MAX_TLS";
static constexpr char kEnvOtlpCipher[]      = "OTEL_CPP_EXPORTER_OTLP_CIPHER";
static constexpr char kEnvOtlpCipherSuite[] = "OTEL_CPP_EXPORTER_OTLP_CIPHER_SUITE";

static bool GetBoolDualEnvVar(const char *signal_name, const char *generic_name, bool &value)
{
  bool exists{};

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
  bool exists{};

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
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetStringEnvironmentVariable(generic_name, value);

  return exists;
}

static bool GetUintDualEnvVar(const char *signal_name,
                              const char *generic_name,
                              std::uint32_t &value)
{
  bool exists{};

  exists = sdk_common::GetUintEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetUintEnvironmentVariable(generic_name, value);

  return exists;
}

static bool GetFloatDualEnvVar(const char *signal_name, const char *generic_name, float &value)
{
  bool exists{};

  exists = sdk_common::GetFloatEnvironmentVariable(signal_name, value);
  if (exists)
  {
    return true;
  }

  exists = sdk_common::GetFloatEnvironmentVariable(generic_name, value);

  return exists;
}

std::string GetOtlpDefaultGrpcTracesEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4317";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpEndpoint, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultGrpcMetricsEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4317";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpEndpoint, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultGrpcLogsEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4317";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpEndpoint, value);

  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpTracesEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4318/v1/traces";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kEnvOtlpEndpoint, value);
  if (exists)
  {
    value += "/v1/traces";
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpMetricsEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4318/v1/metrics";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kEnvOtlpEndpoint, value);
  if (exists)
  {
    value += "/v1/metrics";
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpLogsEndpoint()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_ENDPOINT";
  constexpr char kDefault[]   = "http://localhost:4318/v1/logs";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kEnvOtlpEndpoint, value);
  if (exists)
  {
    value += "/v1/logs";
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpTracesProtocol()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_TRACES_PROTOCOL";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_PROTOCOL";
  constexpr char kDefault[]    = "http/protobuf";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpMetricsProtocol()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_METRICS_PROTOCOL";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_PROTOCOL";
  constexpr char kDefault[]    = "http/protobuf";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return kDefault;
}

std::string GetOtlpDefaultHttpLogsProtocol()
{
  constexpr char kSignalEnv[]  = "OTEL_EXPORTER_OTLP_LOGS_PROTOCOL";
  constexpr char kGenericEnv[] = "OTEL_EXPORTER_OTLP_PROTOCOL";
  constexpr char kDefault[]    = "http/protobuf";

  std::string value;
  bool exists{};

  exists = sdk_common::GetStringEnvironmentVariable(kSignalEnv, value);
  if (exists)
  {
    return value;
  }

  exists = sdk_common::GetStringEnvironmentVariable(kGenericEnv, value);
  if (exists)
  {
    return value;
  }

  return kDefault;
}

bool GetOtlpDefaultGrpcTracesIsInsecure()
{
  std::string endpoint = GetOtlpDefaultGrpcTracesEndpoint();

  /* The trace endpoint, when providing a scheme, takes precedence. */

  if (opentelemetry::nostd::string_view(endpoint).substr(0, 6) == "https:")
  {
    return false;
  }

  if (endpoint.substr(0, 5) == "http:")
  {
    return true;
  }

  constexpr char kSignalEnv[]    = "OTEL_EXPORTER_OTLP_TRACES_INSECURE";
  constexpr char kOldSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_SSL_ENABLE";

  bool insecure{};
  bool ssl_enabled{};
  bool exists{};

  exists = GetBoolDualEnvVar(kSignalEnv, kEnvOtlpInsecure, insecure);
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

  exists = sdk_common::GetBoolEnvironmentVariable(kEnvOtlpSslEnable, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kEnvOtlpSslEnable << "> is deprecated, use <"
                                                    << kEnvOtlpInsecure << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  return false;
}

bool GetOtlpDefaultGrpcMetricsIsInsecure()
{
  std::string endpoint = GetOtlpDefaultGrpcMetricsEndpoint();

  /* The metrics endpoint, when providing a scheme, takes precedence. */

  if (opentelemetry::nostd::string_view(endpoint).substr(0, 6) == "https:")
  {
    return false;
  }

  if (endpoint.substr(0, 5) == "http:")
  {
    return true;
  }

  constexpr char kSignalEnv[]    = "OTEL_EXPORTER_OTLP_METRICS_INSECURE";
  constexpr char kOldSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_SSL_ENABLE";

  bool insecure{};
  bool ssl_enabled{};
  bool exists{};

  exists = GetBoolDualEnvVar(kSignalEnv, kEnvOtlpInsecure, insecure);
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

  exists = sdk_common::GetBoolEnvironmentVariable(kEnvOtlpSslEnable, ssl_enabled);
  if (exists)
  {
#ifdef WARN_DEPRECATED_ENV
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kEnvOtlpSslEnable << "> is deprecated, use <"
                                                    << kEnvOtlpInsecure << "> instead.");
#endif

    insecure = !ssl_enabled;
    return insecure;
  }

  return false;
}

bool GetOtlpDefaultGrpcLogsIsInsecure()
{
  std::string endpoint = GetOtlpDefaultGrpcLogsEndpoint();

  /* The logs endpoint, when providing a scheme, takes precedence. */

  if (opentelemetry::nostd::string_view(endpoint).substr(0, 6) == "https:")
  {
    return false;
  }

  if (endpoint.substr(0, 5) == "http:")
  {
    return true;
  }

  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_INSECURE";

  bool insecure{};
  bool exists{};

  exists = GetBoolDualEnvVar(kSignalEnv, kEnvOtlpInsecure, insecure);
  if (exists)
  {
    return insecure;
  }

  return false;
}

std::string GetOtlpDefaultTracesSslCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientKeyPath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_KEY";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKey, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientKeyPath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_KEY";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKey, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientKeyPath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_KEY";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKey, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientKeyString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_KEY_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKeyString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientKeyString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_KEY_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKeyString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientKeyString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_KEY_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientKeyString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientCertificatePath()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_CERTIFICATE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificate, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslClientCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslClientCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslClientCertificateString()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_CLIENT_CERTIFICATE_STRING";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpClientCertificateString, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

/*
  EXPERIMENTAL:
  Environment variable names do not exist in the spec,
  using the OTEL_CPP_ namespace.
*/

std::string GetOtlpDefaultTracesSslTlsMinVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_MIN_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMinTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslTlsMinVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_MIN_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMinTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslTlsMinVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_MIN_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMinTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslTlsMaxVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_MAX_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMaxTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslTlsMaxVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_MAX_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMaxTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslTlsMaxVersion()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_MAX_TLS";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpMaxTls, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslTlsCipher()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_CIPHER";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipher, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslTlsCipher()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_CIPHER";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipher, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslTlsCipher()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_CIPHER";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipher, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultTracesSslTlsCipherSuite()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_CIPHER_SUITE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipherSuite, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultMetricsSslTlsCipherSuite()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_CIPHER_SUITE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipherSuite, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::string GetOtlpDefaultLogsSslTlsCipherSuite()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_CIPHER_SUITE";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCipherSuite, value);
  if (exists)
  {
    return value;
  }

  return std::string{};
}

std::chrono::system_clock::duration GetOtlpDefaultTracesTimeout()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists{};

  exists = GetDurationDualEnvVar(kSignalEnv, kEnvOtlpTimeout, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

std::chrono::system_clock::duration GetOtlpDefaultMetricsTimeout()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists{};

  exists = GetDurationDualEnvVar(kSignalEnv, kEnvOtlpTimeout, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

std::chrono::system_clock::duration GetOtlpDefaultLogsTimeout()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_TIMEOUT";

  std::chrono::system_clock::duration value;
  bool exists{};

  exists = GetDurationDualEnvVar(kSignalEnv, kEnvOtlpTimeout, value);
  if (exists)
  {
    return value;
  }

  value = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
  return value;
}

bool GetOtlpDefaultTracesTimeoutOverride(std::chrono::system_clock::duration &value)
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_TIMEOUT";
  return sdk_common::GetDurationEnvironmentVariable(kSignalEnv, value);
}

bool GetOtlpDefaultMetricsTimeoutOverride(std::chrono::system_clock::duration &value)
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_TIMEOUT";
  return sdk_common::GetDurationEnvironmentVariable(kSignalEnv, value);
}

bool GetOtlpDefaultLogsTimeoutOverride(std::chrono::system_clock::duration &value)
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_TIMEOUT";
  return sdk_common::GetDurationEnvironmentVariable(kSignalEnv, value);
}

void DumpOtlpHeaders(OtlpHeaders &output, const char *env_var_name)
{
  std::string raw_value;
  bool exists{};

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
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_HEADERS";

  return GetHeaders(kSignalEnv, kEnvOtlpHeaders);
}

OtlpHeaders GetOtlpDefaultMetricsHeaders()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_HEADERS";

  return GetHeaders(kSignalEnv, kEnvOtlpHeaders);
}

OtlpHeaders GetOtlpDefaultLogsHeaders()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_HEADERS";

  return GetHeaders(kSignalEnv, kEnvOtlpHeaders);
}

std::string GetOtlpDefaultTracesCompression()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_TRACES_COMPRESSION";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCompression, value);
  if (exists)
  {
    return value;
  }

  return std::string{"none"};
}

std::string GetOtlpDefaultMetricsCompression()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_METRICS_COMPRESSION";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCompression, value);
  if (exists)
  {
    return value;
  }

  return std::string{"none"};
}

std::string GetOtlpDefaultLogsCompression()
{
  constexpr char kSignalEnv[] = "OTEL_EXPORTER_OTLP_LOGS_COMPRESSION";

  std::string value;
  bool exists{};

  exists = GetStringDualEnvVar(kSignalEnv, kEnvOtlpCompression, value);
  if (exists)
  {
    return value;
  }

  return std::string{"none"};
}

std::uint32_t GetOtlpDefaultTracesRetryMaxAttempts()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_ATTEMPTS";
  std::uint32_t value{};

  if (GetUintDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxAttempts, value))
  {
    return value;
  }

  return 5U;
}

std::uint32_t GetOtlpDefaultMetricsRetryMaxAttempts()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_ATTEMPTS";
  std::uint32_t value{};

  if (GetUintDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxAttempts, value))
  {
    return value;
  }

  return 5U;
}

std::uint32_t GetOtlpDefaultLogsRetryMaxAttempts()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_RETRY_MAX_ATTEMPTS";
  std::uint32_t value{};

  if (GetUintDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxAttempts, value))
  {
    return value;
  }

  return 5U;
}

std::chrono::duration<float> GetOtlpDefaultTracesRetryInitialBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_INITIAL_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryInitialBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{1.0f};
}

std::chrono::duration<float> GetOtlpDefaultMetricsRetryInitialBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_INITIAL_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryInitialBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{1.0f};
}

std::chrono::duration<float> GetOtlpDefaultLogsRetryInitialBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_RETRY_INITIAL_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryInitialBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{1.0f};
}

std::chrono::duration<float> GetOtlpDefaultTracesRetryMaxBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{5.0f};
}

std::chrono::duration<float> GetOtlpDefaultMetricsRetryMaxBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_MAX_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{5.0f};
}

std::chrono::duration<float> GetOtlpDefaultLogsRetryMaxBackoff()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_RETRY_MAX_BACKOFF";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryMaxBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{5.0f};
}

float GetOtlpDefaultTracesRetryBackoffMultiplier()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_BACKOFF_MULTIPLIER";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryBackoffMultiplier, value))
  {
    return value;
  }

  return 1.5f;
}

float GetOtlpDefaultMetricsRetryBackoffMultiplier()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_METRICS_RETRY_BACKOFF_MULTIPLIER";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryBackoffMultiplier, value))
  {
    return value;
  }

  return 1.5f;
}

float GetOtlpDefaultLogsRetryBackoffMultiplier()
{
  constexpr char kSignalEnv[] = "OTEL_CPP_EXPORTER_OTLP_LOGS_RETRY_BACKOFF_MULTIPLIER";
  float value{};

  if (GetFloatDualEnvVar(kSignalEnv, kEnvOtlpRetryBackoffMultiplier, value))
  {
    return value;
  }

  return 1.5f;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientEndpoint()
{
  constexpr char kDefault[] = "http://localhost:4317";

  std::string value;

  if (sdk_common::GetStringEnvironmentVariable(kEnvOtlpEndpoint, value))
  {
    return value;
  }

  return kDefault;
}

OPENTELEMETRY_EXPORT bool GetOtlpDefaultGrpcClientIsInsecure()
{
  std::string endpoint = GetOtlpDefaultGrpcClientEndpoint();

  if (opentelemetry::nostd::string_view(endpoint).substr(0, 6) == "https:")
  {
    return false;
  }

  if (endpoint.substr(0, 5) == "http:")
  {
    return true;
  }

  bool insecure{};
  bool ssl_enabled{};

  if (sdk_common::GetBoolEnvironmentVariable(kEnvOtlpInsecure, insecure))
  {
    return insecure;
  }

  if (sdk_common::GetBoolEnvironmentVariable(kEnvOtlpSslEnable, ssl_enabled))
  {
    return !ssl_enabled;
  }

  return false;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslCertificatePath()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpCertificate, value);
  return value;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslCertificateString()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpCertificateString, value);
  return value;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslClientKeyPath()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpClientKey, value);
  return value;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslClientKeyString()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpClientKeyString, value);
  return value;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslClientCertificatePath()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpClientCertificate, value);
  return value;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientSslClientCertificateString()
{
  std::string value;
  sdk_common::GetStringEnvironmentVariable(kEnvOtlpClientCertificateString, value);
  return value;
}

OPENTELEMETRY_EXPORT std::chrono::system_clock::duration GetOtlpDefaultGrpcClientTimeout()
{
  std::chrono::system_clock::duration value;

  if (sdk_common::GetDurationEnvironmentVariable(kEnvOtlpTimeout, value))
  {
    return value;
  }

  return std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{10});
}

OPENTELEMETRY_EXPORT OtlpHeaders GetOtlpDefaultGrpcClientHeaders()
{
  OtlpHeaders result;
  DumpOtlpHeaders(result, kEnvOtlpHeaders);
  return result;
}

OPENTELEMETRY_EXPORT std::string GetOtlpDefaultGrpcClientCompression()
{
  std::string value;

  if (sdk_common::GetStringEnvironmentVariable(kEnvOtlpCompression, value))
  {
    return value;
  }

  return std::string{"none"};
}

OPENTELEMETRY_EXPORT std::uint32_t GetOtlpDefaultGrpcClientRetryMaxAttempts()
{
  std::uint32_t value{};

  if (sdk_common::GetUintEnvironmentVariable(kEnvOtlpRetryMaxAttempts, value))
  {
    return value;
  }

  return 5U;
}

OPENTELEMETRY_EXPORT std::chrono::duration<float> GetOtlpDefaultGrpcClientRetryInitialBackoff()
{
  float value{};

  if (sdk_common::GetFloatEnvironmentVariable(kEnvOtlpRetryInitialBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{1.0f};
}

OPENTELEMETRY_EXPORT std::chrono::duration<float> GetOtlpDefaultGrpcClientRetryMaxBackoff()
{
  float value{};

  if (sdk_common::GetFloatEnvironmentVariable(kEnvOtlpRetryMaxBackoff, value))
  {
    return std::chrono::duration<float>{value};
  }

  return std::chrono::duration<float>{5.0f};
}

OPENTELEMETRY_EXPORT float GetOtlpDefaultGrpcClientRetryBackoffMultiplier()
{
  float value{};

  if (sdk_common::GetFloatEnvironmentVariable(kEnvOtlpRetryBackoffMultiplier, value))
  {
    return value;
  }

  return 1.5f;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
