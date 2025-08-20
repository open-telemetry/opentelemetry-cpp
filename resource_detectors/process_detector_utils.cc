// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/process_detector_utils.h"

#include <fstream>
#include <string>

#ifdef _MSC_VER
// clang-format off
#  include <windows.h>
#  include <psapi.h>
// clang-format on
#else
#  include <sys/types.h>
#  include <unistd.h>
#  include <cstdio>
#endif

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

constexpr const char *kExecutableName = "exe";
constexpr const char *kCmdlineName    = "cmdline";

std::string GetExecutablePath(const int32_t &pid)
{
#ifdef _MSC_VER
  HANDLE hProcess =
      OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
  if (!hProcess)
  {
    return std::string();
  }

  WCHAR wbuffer[MAX_PATH];
  DWORD len = GetProcessImageFileNameW(hProcess, wbuffer, MAX_PATH);
  CloseHandle(hProcess);

  if (len == 0)
  {
    return std::string();
  }

  // Convert UTF-16 to UTF-8
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, NULL, 0, NULL, NULL);
  std::string utf8_path(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, &utf8_path[0], size_needed, NULL, NULL);

  return utf8_path;
#else
  std::string path = FormFilePath(pid, kExecutableName);
  char buffer[4096];

  ssize_t len = readlink(path.c_str(), buffer, sizeof(buffer) - 1);
  if (len != -1)
  {
    buffer[len] = '\0';
    return std::string(buffer);
  }

  return std::string();
#endif
}

std::string GetCommand(const int32_t &pid)
{
#ifdef _MSC_VER
  // On Windows, GetCommandLineW only works for the CURRENT process,
  // so we ignore `pid` and just return the current process's command line.
  LPCWSTR wcmd = GetCommandLineW();
  if (!wcmd)
  {
    return std::string();
  }

  // Convert UTF-16 to UTF-8
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, NULL, 0, NULL, NULL);
  if (size_needed <= 0)
  {
    return std::string();
  }

  std::string utf8_command(size_needed - 1, 0);  // exclude null terminator
  WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, &utf8_command[0], size_needed, NULL, NULL);

  return utf8_command;
#else
  // This is the path to get the command that was used to start the process
  std::string command_line_path = FormFilePath(pid, kCmdlineName);
  return ExtractCommand(command_line_path);
#endif
}

std::string ExtractCommand(const std::string &command_line_path)
{
  std::string command;
  std::ifstream command_line_file(command_line_path, std::ios::in | std::ios::binary);
  std::getline(command_line_file, command, '\0');
  return command;
}

std::string FormFilePath(const int32_t &pid, const char *process_type)
{
  char buff[64];
  int len = std::snprintf(buff, sizeof(buff), "/proc/%d/%s", pid, process_type);
  if (len < 0)
  {
    // in case snprintf fails
    return std::string();
  }
  if (len >= static_cast<int>(sizeof(buff)))
  {
    return std::string(buff, sizeof(buff) - 1);
  }
  return std::string(buff, len);
}

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
