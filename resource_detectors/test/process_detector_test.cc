// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <fstream>
#include <string>

#ifdef _MSC_VER
// clang-format off
#  include <process.h>
#  include <windows.h>
#  include <psapi.h>
#  define getpid _getpid
// clang-format on
#else
#  include <sys/types.h>
#  include <unistd.h>
#  include <cstdio>
#endif

#include "opentelemetry/resource_detectors/detail/process_detector_utils.h"

TEST(ProcessDetectorUtilsTest, FormFilePath)
{
  int32_t pid              = 1234;
  std::string cmdline_path = opentelemetry::resource_detector::detail::FormFilePath(pid, "cmdline");
  std::string exe_path     = opentelemetry::resource_detector::detail::FormFilePath(pid, "exe");

  EXPECT_EQ(cmdline_path, "/proc/1234/cmdline");
  EXPECT_EQ(exe_path, "/proc/1234/exe");
}

TEST(ProcessDetectorUtilsTest, ExtractCommand)
{
  std::string filename{"test_command.txt"};

  {
    std::ofstream outfile(filename, std::ios::binary);
    const char raw_data[] = "test_command\0arg1\0arg2\0arg3\0";
    outfile.write(raw_data, sizeof(raw_data) - 1);
  }

  std::string command = opentelemetry::resource_detector::detail::ExtractCommand(filename);
  EXPECT_EQ(command, std::string{"test_command"});

  std::remove(filename.c_str());  // Cleanup
}

TEST(ProcessDetectorUtilsTest, EmptyCommandFile)
{
  std::string filename{"empty_command.txt"};
  std::ofstream outfile(filename, std::ios::binary);
  outfile.close();

  std::string command = opentelemetry::resource_detector::detail::ExtractCommand(filename);
  EXPECT_EQ(command, std::string{""});

  std::remove(filename.c_str());  // Cleanup
}

TEST(ProcessDetectorUtilsTest, GetExecutablePathTest)
{
  int32_t pid = getpid();
  std::string path;
#ifdef _MSC_VER
  HANDLE hProcess =
      OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
  if (!hProcess)
  {
    path = std::string();
  }
  else
  {

    WCHAR wbuffer[MAX_PATH];
    DWORD len = GetProcessImageFileNameW(hProcess, wbuffer, MAX_PATH);
    CloseHandle(hProcess);

    if (len == 0)
    {
      path = std::string();
    }
    else
    {
      int size_needed = WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, NULL, 0, NULL, NULL);
      std::string utf8_path(size_needed, 0);
      WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, &utf8_path[0], size_needed, NULL, NULL);

      path = utf8_path;
    }
  }
#else
  std::string exe_path = opentelemetry::resource_detector::detail::FormFilePath(pid, "exe");
  char buffer[4096];

  ssize_t len = readlink(exe_path.c_str(), buffer, sizeof(buffer) - 1);
  if (len != -1)
  {
    buffer[len] = '\0';
    path        = std::string(buffer);
  }
  else
  {
    path = std::string();
  }
#endif
  std::string expected_path = opentelemetry::resource_detector::detail::GetExecutablePath(pid);
  EXPECT_EQ(path, expected_path);
}

TEST(ProcessDetectorUtilsTest, GetCommandTest)
{
  int32_t pid = getpid();
  std::string command;
#ifdef _MSC_VER
  // On Windows, GetCommandLineW only works for the CURRENT process,
  // so we ignore `pid` and just return the current process's command line.
  LPCWSTR wcmd = GetCommandLineW();
  if (!wcmd)
  {
    command = std::string();
  }
  else
  {

    // Convert UTF-16 to UTF-8
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, NULL, 0, NULL, NULL);
    if (size_needed <= 0)
    {
      command = std::string();
    }
    else
    {
      std::string utf8_command(size_needed - 1, 0);  // exclude null terminator
      WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, &utf8_command[0], size_needed, NULL, NULL);
      command = utf8_command;
    }
  }
#else
  // This is the path to get the command that was used to start the process
  std::string command_line_path =
      opentelemetry::resource_detector::detail::FormFilePath(pid, "cmdline");
  command = opentelemetry::resource_detector::detail::ExtractCommand(command_line_path);
#endif
  std::string expected_command = opentelemetry::resource_detector::detail::GetCommand(pid);
  EXPECT_EQ(command, expected_command);
}
