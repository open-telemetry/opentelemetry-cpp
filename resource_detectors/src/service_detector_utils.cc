// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/service_detector_utils.h"

#include <cstdint>
#include <mutex>
#include <string>

#ifdef _MSC_VER
#  include <process.h>
#  define getpid _getpid
#else
#  include <unistd.h>
#endif

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/resource_detectors/detail/process_detector_utils.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/version.h"
#include "src/common/random.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

namespace
{
constexpr const char *kOtelServiceName      = "OTEL_SERVICE_NAME";
constexpr const char *kUnknownService       = "unknown_service";
constexpr const char *kUnknownServicePrefix = "unknown_service:";
constexpr const char kUuidHexDigits[]       = "0123456789abcdef";

std::string FormatUuidV4(const uint8_t bytes[16]) noexcept
{
  std::string uuid;
  uuid.reserve(36);
  for (size_t i = 0; i < 16; ++i)
  {
    if (i == 4 || i == 6 || i == 8 || i == 10)
    {
      uuid.push_back('-');
    }
    uuid.push_back(kUuidHexDigits[bytes[i] >> 4]);
    uuid.push_back(kUuidHexDigits[bytes[i] & 0x0F]);
  }
  return uuid;
}

std::string GenerateUuidV4() noexcept
{
  uint8_t bytes[16];
  opentelemetry::sdk::common::Random::GenerateRandomBuffer(
      opentelemetry::nostd::span<uint8_t>(bytes));
  bytes[6] = static_cast<uint8_t>((bytes[6] & 0x0F) | 0x40);
  bytes[8] = static_cast<uint8_t>((bytes[8] & 0x3F) | 0x80);
  return FormatUuidV4(bytes);
}

}  // namespace

std::string GetServiceName()
{
  std::string service_name;
  if (opentelemetry::sdk::common::GetStringEnvironmentVariable(kOtelServiceName, service_name) &&
      !service_name.empty())
  {
    return service_name;
  }

  const std::string executable_name = GetExecutableName(static_cast<int32_t>(getpid()));
  if (!executable_name.empty())
  {
    std::string fallback_service_name(kUnknownServicePrefix);
    fallback_service_name.append(executable_name);
    return fallback_service_name;
  }

  return std::string{kUnknownService};
}

std::string GenerateServiceInstanceId() noexcept
{
  static std::mutex mutex;
  static int32_t cached_pid = -1;
  static std::string cached_id;

  const int32_t pid = static_cast<int32_t>(getpid());
  const std::lock_guard<std::mutex> lock(mutex);
  if (cached_pid != pid || cached_id.empty())
  {
    cached_pid = pid;
    cached_id  = GenerateUuidV4();
  }
  return cached_id;
}

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
