// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/service_detector_utils.h"

#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>

#ifdef _MSC_VER
#  include <process.h>
#  define getpid _getpid
#else
#  include <unistd.h>
#endif

#include "opentelemetry/common/detail/ascii.h"
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

std::string FormatUuidV4(const uint8_t bytes[16]) noexcept
{
  // Groups of 8, 4, 4, 4 and 12 hexadecimal digits, separated by dashes.
  std::string uuid(36, '-');
  opentelemetry::common::detail::EncodeLowerHex(&bytes[0], 4, &uuid[0]);
  opentelemetry::common::detail::EncodeLowerHex(&bytes[4], 2, &uuid[9]);
  opentelemetry::common::detail::EncodeLowerHex(&bytes[6], 2, &uuid[14]);
  opentelemetry::common::detail::EncodeLowerHex(&bytes[8], 2, &uuid[19]);
  opentelemetry::common::detail::EncodeLowerHex(&bytes[10], 6, &uuid[24]);
  return uuid;
}

std::string GenerateUuidV4() noexcept
{
  uint8_t bytes[16];
  for (size_t i = 0; i < sizeof(bytes); i += sizeof(uint64_t))
  {
    uint64_t value = opentelemetry::sdk::common::Random::GenerateRandom64();
    std::memcpy(&bytes[i], &value, sizeof(uint64_t));
  }
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

  const auto exe_info                = GetExecutableInfo(static_cast<int32_t>(getpid()));
  const std::string &executable_name = exe_info.name;

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
