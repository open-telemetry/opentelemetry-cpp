// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/env_variables.h"

#ifdef _MSC_VER
#  include <string.h>
#  define strcasecmp _stricmp
#else
#  include <strings.h>
#endif

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <ostream>
#include <type_traits>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

template <typename T, typename U, std::enable_if_t<std::is_integral<U>::value, bool> = true>
constexpr bool ParseNumber(T &&input, U &output)
{
  constexpr auto max_value = std::numeric_limits<U>::max();

  // Skip spaces
  for (; *input && std::isspace(*input); ++input)
    ;

  const auto is_negative = (*input && *input == '-');

  if (is_negative)
  {
    ++input;

    // Reject negative when expecting unsigned
    if (std::is_unsigned<U>::value)
    {
      return false;
    }
  }

  const auto pos = input;
  U result       = 0;
  U temp         = 0;

  for (; *input && std::isdigit(*input); ++input)
  {
    temp = result * 10 + (*input - '0');

    if (max_value - temp > max_value - result)
    {
      return false;  // Overflow protection
    }

    result = temp;
  }

  result *= (is_negative) ? -1 : 1;

  // Reject if nothing parsed
  if (pos != input)
  {
    output = result;
    return true;
  }

  return false;
}

template <typename T, typename U, std::enable_if_t<std::is_floating_point<U>::value, bool> = true>
constexpr bool ParseNumber(T &&input, U &output)
{
  // Skip spaces
  for (; *input && std::isspace(*input); ++input)
    ;

  const auto is_negative = (*input && *input == '-');

  if (is_negative)
  {
    ++input;
  }

  const auto pos  = input;
  U result        = 0;
  U temp          = 0;
  U decimal_mul   = 0.1f;
  bool is_decimal = false;

  for (; *input && (std::isdigit(*input) || !is_decimal); ++input)
  {
    if (*input == '.' && !is_decimal)
    {
      is_decimal = true;
      continue;
    }
    else if (*input == '.' && is_decimal)
    {
      break;
    }
    else if (is_decimal)
    {
      temp = result + decimal_mul * (*input - '0');
      decimal_mul *= 0.1f;
    }
    else
    {
      temp = result * 10 + (*input - '0');
    }

    if (std::isinf(temp))
    {
      return false;  // Overflow protection
    }

    result = temp;
  }

  result *= (is_negative) ? -1.0f : 1.0f;

  // Reject if nothing parsed
  if (pos != input && !std::isnan(output) && !std::isinf(output))
  {
    output = result;
    return true;
  }

  return false;
}

bool GetRawEnvironmentVariable(const char *env_var_name, std::string &value)
{
#if !defined(NO_GETENV)
  const char *endpoint_from_env = nullptr;
#  if defined(_MSC_VER)
  // avoid calling std::getenv which is deprecated in MSVC.
  size_t required_size = 0;
  getenv_s(&required_size, nullptr, 0, env_var_name);
  std::unique_ptr<char[]> endpoint_buffer;
  if (required_size > 0)
  {
    endpoint_buffer = std::unique_ptr<char[]>{new char[required_size]};
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
  if (!exists || raw_value.empty())
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

static bool GetTimeoutFromString(const char *input, std::chrono::system_clock::duration &value)
{
  std::chrono::system_clock::duration::rep result = 0;

  if (!ParseNumber(input, result))
  {
    // Rejecting duration 0 as invalid.
    return false;
  }

  opentelemetry::nostd::string_view unit{input};

  if (unit == "ns")
  {
    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::nanoseconds{result});
    return true;
  }

  if (unit == "us")
  {
    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::microseconds{result});
    return true;
  }

  if (unit == "ms")
  {
    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::milliseconds{result});
    return true;
  }

  if (unit == "s")
  {
    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::seconds{result});
    return true;
  }

  if (unit == "m")
  {
    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::minutes{result});
    return true;
  }

  if (unit == "h")
  {
    value =
        std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::hours{result});
    return true;
  }

  if (unit == "")
  {
    // TODO: The spec says milliseconds, but opentelemetry-cpp implemented
    // seconds by default. Fixing this is a breaking change.

    value = std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::seconds{result});
    return true;
  }

  // Failed to parse the input string.
  return false;
}

bool GetDurationEnvironmentVariable(const char *env_var_name,
                                    std::chrono::system_clock::duration &value)
{
  std::string raw_value;
  bool exists = GetRawEnvironmentVariable(env_var_name, raw_value);
  if (!exists || raw_value.empty())
  {
    value =
        std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{0});
    return false;
  }

  exists = GetTimeoutFromString(raw_value.c_str(), value);

  if (!exists)
  {
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << env_var_name << "> has an invalid value <"
                                                    << raw_value << ">, ignoring");
  }
  return exists;
}

bool GetStringEnvironmentVariable(const char *env_var_name, std::string &value)
{
  bool exists = GetRawEnvironmentVariable(env_var_name, value);
  if (!exists || value.empty())
  {
    return false;
  }
  return true;
}

bool GetUintEnvironmentVariable(const char *env_var_name, std::uint32_t &value)
{
  static constexpr auto kDefaultValue = 0U;
  std::string raw_value;
  bool exists = GetRawEnvironmentVariable(env_var_name, raw_value);
  if (!exists || raw_value.empty())
  {
    value = kDefaultValue;
    return false;
  }

  if (!ParseNumber(raw_value.c_str(), value))
  {
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << env_var_name << "> has an invalid value <"
                                                    << raw_value << ">, defaulting to "
                                                    << kDefaultValue);
    value = kDefaultValue;
    return false;
  }

  return true;
}

bool GetFloatEnvironmentVariable(const char *env_var_name, float &value)
{
  static constexpr auto kDefaultValue = 0.0f;
  std::string raw_value;
  bool exists = GetRawEnvironmentVariable(env_var_name, raw_value);
  if (!exists || raw_value.empty())
  {
    value = kDefaultValue;
    return false;
  }

  if (!ParseNumber(raw_value.c_str(), value))
  {
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << env_var_name << "> has an invalid value <"
                                                    << raw_value << ">, defaulting to "
                                                    << kDefaultValue);
    value = kDefaultValue;
    return false;
  }

  return true;
}

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
