// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/env_variables.h"

#ifdef _MSC_VER
#  include <string.h>
#  define strcasecmp _stricmp
#else
#  include <strings.h>
#endif

#include <memory>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

bool GetRawEnvironmentVariable(const char *env_var_name, std::string &value)
{
#if !defined(NO_GETENV)
  const char *endpoint_from_env = nullptr;
#  if defined(_MSC_VER)
  // avoid calling std::getenv which is deprecated in MSVC.
  size_t required_size = 0;
  getenv_s(&required_size, nullptr, 0, env_var_name);
  std::unique_ptr<char> endpoint_buffer;
  if (required_size > 0)
  {
    endpoint_buffer = std::unique_ptr<char>{new char[required_size]};
    getenv_s(&required_size, endpoint_buffer.get(), required_size, env_var_name);
    endpoint_from_env = endpoint_buffer.get();
  }
#  else
  endpoint_from_env = std::getenv(env_var_name);
#  endif  // defined(_MSC_VER)

  if (endpoint_from_env != nullptr)
  {
    value = std::string{endpoint_from_env};
    return true;
  }

  value = std::string{};
  return false;
#else
  value = std::string{};
  return false;
#endif  // !defined(NO_GETENV)
}

bool GetBoolEnvironmentVariable(const char *env_var_name, bool &value)
{
  std::string raw_value;
  bool exists = GetRawEnvironmentVariable(env_var_name, raw_value);
  if (!exists)
  {
    value = false;
    return false;
  }

  if (strcasecmp(raw_value.c_str(), "true") == 0)
  {
    value = true;
    return true;
  }

  if (strcasecmp(raw_value.c_str(), "false") == 0)
  {
    value = false;
    return true;
  }

  OTEL_INTERNAL_LOG_WARN("Environment variable <" << env_var_name << "> has an invalid value <"
                                                  << raw_value << ">, defaulting to false");
  value = false;
  return true;
}

static std::chrono::system_clock::duration GetTimeoutFromString(const char *input)
{
  std::chrono::system_clock::duration::rep result = 0;

  // Skip spaces
  for (; *input && (' ' == *input || '\t' == *input || '\r' == *input || '\n' == *input); ++input)
    ;

  for (; *input && (*input >= '0' && *input <= '9'); ++input)
  {
    result = result * 10 + (*input - '0');
  }

  opentelemetry::nostd::string_view unit{input};

  if (unit == "ns")
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::nanoseconds{result});
  }

  if (unit == "us")
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::microseconds{result});
  }

  if (unit == "ms")
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::milliseconds{result});
  }

  if (unit == "m")
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::minutes{result});
  }

  if (unit == "h")
  {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::hours{result});
  }

  return std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::seconds{result});
}

bool GetDurationEnvironmentVariable(const char *env_var_name,
                                    std::chrono::system_clock::duration &value)
{
  std::string raw_value;
  bool exists = GetRawEnvironmentVariable(env_var_name, raw_value);
  if (!exists)
  {
    value =
        std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{0});
    return false;
  }

  value = GetTimeoutFromString(raw_value.c_str());
  return true;
}

bool GetStringEnvironmentVariable(const char *env_var_name, std::string &value)
{
  return GetRawEnvironmentVariable(env_var_name, value);
}

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
