#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <opentelemetry/version.h>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace utils
{

///
/// Convert from time_point to ISO string
///
static std::string to_string(std::chrono::system_clock::time_point &tp)
{
  int64_t millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
  auto in_time_t = std::chrono::system_clock::to_time_t(tp);
  std::stringstream ss;
  // TODO: this is expected to be UTC time
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H:%M:%S");
  ss << "." << std::setfill('0') << std::setw(3) << (unsigned)(millis % 1000);
  ss << "Z";
  return ss.str();
}

/// <summary>
/// Compile-time constexpr djb2 hash function for strings
/// </summary>
static constexpr uint32_t hashCode(const char *str, uint32_t h = 0)
{
  return (uint32_t)(!str[h] ? 5381 : ((uint32_t)hashCode(str, h + 1) * (uint32_t)33) ^ str[h]);
}

#define CONST_UINT32_T(x) std::integral_constant<uint32_t, (uint32_t)x>::value

#define CONST_HASHCODE(name) CONST_UINT32_T(OPENTELEMETRY_NAMESPACE::utils::hashCode(#name))

};  // namespace utils

OPENTELEMETRY_END_NAMESPACE
