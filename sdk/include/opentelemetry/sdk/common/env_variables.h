// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

#if defined(_MSC_VER)
inline int setenv(const char *name, const char *value, int)
{
  return _putenv_s(name, value);
}

inline int unsetenv(const char *name)
{
  return setenv(name, "", 1);
}
#endif

// Returns the env variable set.
inline const std::string GetEnvironmentVariable(const char *env_var_name)
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
  return endpoint_from_env == nullptr ? std::string{} : std::string{endpoint_from_env};
#else
  return std::string{};
#endif  // !defined(NO_GETENV)
}
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
