// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/host_detector_utils.h"

#include <fstream>
#include <string>

#ifdef _MSC_VER
// clang-format off
#  include <windows.h>
#  pragma comment(lib, "advapi32.lib")
// clang-format on
#elif defined(__APPLE__)
#  include <unistd.h>
#  include <uuid/uuid.h>
#  include <ctime>
#else
#  include <unistd.h>
#endif

#include "opentelemetry/semconv/incubating/host_attributes.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

#if !defined(_MSC_VER) && !defined(__APPLE__)
constexpr const char *kMachineIdPath     = "/etc/machine-id";
constexpr const char *kDbusMachineIdPath = "/var/lib/dbus/machine-id";
#endif

std::string GetHostName()
{
#ifdef _MSC_VER
  DWORD size = 0;
  GetComputerNameExW(ComputerNameDnsHostname, nullptr, &size);
  if (size == 0)
  {
    return std::string();
  }

  std::wstring wname(size, L'\0');
  if (!GetComputerNameExW(ComputerNameDnsHostname, &wname[0], &size))
  {
    return std::string();
  }

  // Convert UTF-16 to UTF-8
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), static_cast<int>(size), nullptr,
                                        0, nullptr, nullptr);
  if (size_needed <= 0)
  {
    return std::string();
  }
  std::string utf8_name(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), static_cast<int>(size), &utf8_name[0], size_needed,
                      nullptr, nullptr);
  return utf8_name;
#else
  char buffer[256];
  if (gethostname(buffer, sizeof(buffer)) != 0)
  {
    return std::string();
  }
  buffer[sizeof(buffer) - 1] = '\0';
  return std::string(buffer);
#endif
}

std::string GetHostArch()
{
#if defined(_M_AMD64) || defined(__x86_64__)
  return semconv::host::HostArchValues::kAmd64;
#elif defined(_M_ARM64) || defined(__aarch64__)
  return semconv::host::HostArchValues::kArm64;
#elif defined(_M_ARM) || defined(__arm__)
  return semconv::host::HostArchValues::kArm32;
#elif defined(_M_IX86) || defined(__i386__)
  return semconv::host::HostArchValues::kX86;
#elif defined(__powerpc64__)
  return semconv::host::HostArchValues::kPpc64;
#elif defined(__powerpc__)
  return semconv::host::HostArchValues::kPpc32;
#elif defined(__s390x__)
  return semconv::host::HostArchValues::kS390x;
#elif defined(__ia64__)
  return semconv::host::HostArchValues::kIa64;
#else
  return std::string();
#endif
}

std::string GetHostIdFromFile(const char *file_path)
{
  std::ifstream file(file_path);
  if (!file.is_open())
  {
    return std::string();
  }

  std::string line;
  std::getline(file, line);

  const auto end = line.find_last_not_of(" \t\n\r");
  if (end == std::string::npos)
  {
    return std::string();
  }
  const auto start = line.find_first_not_of(" \t\n\r");
  return line.substr(start, end - start + 1);
}

std::string GetHostId()
{
#ifdef _MSC_VER
  char buffer[64];
  DWORD size     = sizeof(buffer);
  LSTATUS status = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography",
                                "MachineGuid", RRF_RT_REG_SZ, nullptr, buffer, &size);
  if (status != ERROR_SUCCESS || size <= 1)
  {
    return std::string();
  }
  // size includes the terminating null character
  return std::string(buffer, size - 1);
#elif defined(__APPLE__)
  uuid_t uuid;
  struct timespec timeout = {1, 0};
  if (gethostuuid(uuid, &timeout) != 0)
  {
    return std::string();
  }
  char uuid_str[37];
  uuid_unparse_lower(uuid, uuid_str);
  return std::string(uuid_str);
#else
  std::string machine_id = GetHostIdFromFile(kMachineIdPath);
  if (machine_id.empty())
  {
    machine_id = GetHostIdFromFile(kDbusMachineIdPath);
  }
  return machine_id;
#endif
}

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
