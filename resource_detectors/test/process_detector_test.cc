// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

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
#include "opentelemetry/resource_detectors/process_detector.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/incubating/process_attributes.h"

TEST(ProcessDetectorUtilsTest, FormFilePath)
{
  int32_t pid              = 1234;
  std::string cmdline_path = opentelemetry::resource_detector::detail::FormFilePath(pid, "cmdline");
  std::string exe_path     = opentelemetry::resource_detector::detail::FormFilePath(pid, "exe");

  EXPECT_EQ(cmdline_path, "/proc/1234/cmdline");
  EXPECT_EQ(exe_path, "/proc/1234/exe");
}

TEST(ProcessDetectorUtilsTest, ExtractCommandWithArgs)
{
  std::string filename{"test_command_args.txt"};

  {
    std::ofstream outfile(filename, std::ios::binary);
    const char raw_data[] = "test_command\0arg1\0arg2\0arg3\0";
    outfile.write(raw_data, sizeof(raw_data) - 1);
  }

  std::vector<std::string> args =
      opentelemetry::resource_detector::detail::ExtractCommandWithArgs(filename);
  EXPECT_EQ(args, (std::vector<std::string>{"test_command", "arg1", "arg2", "arg3"}));

  std::remove(filename.c_str());  // Cleanup
}

TEST(ProcessDetectorUtilsTest, EmptyCommandWithArgsFile)
{
  std::string filename{"empty_command_args.txt"};
  std::ofstream outfile(filename, std::ios::binary);
  outfile.close();

  std::vector<std::string> args =
      opentelemetry::resource_detector::detail::ExtractCommandWithArgs(filename);
  EXPECT_TRUE(args.empty());

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
  std::string expected_path = opentelemetry::resource_detector::detail::GetExecutableInfo(pid).path;
  EXPECT_EQ(path, expected_path);
}

TEST(ProcessDetectorUtilsTest, CommandTest)
{
  int32_t pid = getpid();
  std::string command;
#ifdef _MSC_VER
  int argc      = 0;
  LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

  if (argvW && argc > 0)
  {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvW[0], -1, NULL, 0, NULL, NULL);
    if (size_needed > 0)
    {
      std::string arg(size_needed - 1, 0);
      WideCharToMultiByte(CP_UTF8, 0, argvW[0], -1, &arg[0], size_needed, NULL, NULL);
      command = arg;
    }

    LocalFree(argvW);
  }
  else
  {
    command = std::string();
  }
#else
  std::string command_line_path =
      opentelemetry::resource_detector::detail::FormFilePath(pid, "cmdline");
  std::ifstream command_line_file(command_line_path, std::ios::in | std::ios::binary);
  std::getline(command_line_file, command, '\0');
#endif
  std::vector<std::string> expected_command_with_args =
      opentelemetry::resource_detector::detail::GetCommandWithArgs(pid);
  std::string expected_command;
  if (!expected_command_with_args.empty())
  {
    expected_command = expected_command_with_args[0];
  }
  EXPECT_EQ(command, expected_command);
}

TEST(ProcessDetectorUtilsTest, GetCommandWithArgsTest)
{
  int32_t pid = getpid();
  std::vector<std::string> args;
#ifdef _MSC_VER
  int argc      = 0;
  LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argvW)
  {
    args = {};
  }
  else
  {
    for (int i = 0; i < argc; i++)
    {
      // Convert UTF-16 to UTF-8
      int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
      if (size_needed > 0)
      {
        std::string arg(size_needed - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, &arg[0], size_needed, NULL, NULL);
        args.push_back(arg);
      }
    }
  }

  LocalFree(argvW);
#else
  std::string command_line_path =
      opentelemetry::resource_detector::detail::FormFilePath(pid, "cmdline");
  args = opentelemetry::resource_detector::detail::ExtractCommandWithArgs(command_line_path);
#endif
  std::vector<std::string> expected_args =
      opentelemetry::resource_detector::detail::GetCommandWithArgs(pid);
  EXPECT_EQ(args, expected_args);
}

// ---------------------------------------------------------------------------
// New utility tests
// ---------------------------------------------------------------------------

TEST(ProcessDetectorUtilsTest, GetExecutableNameTest)
{
  int32_t pid          = getpid();
  auto exe_info        = opentelemetry::resource_detector::detail::GetExecutableInfo(pid);
  std::string exe_path = exe_info.path;
  std::string exe_name = exe_info.name;

  if (exe_path.empty())
  {
    EXPECT_TRUE(exe_name.empty()) << "Name should be empty when path is empty";
    return;
  }

  EXPECT_FALSE(exe_name.empty()) << "Executable name must not be empty when path is available";
  // The name must be a suffix of the path.
  EXPECT_NE(exe_path.find(exe_name), std::string::npos)
      << "Executable name '" << exe_name << "' should be a substring of path '" << exe_path << "'";
  // The name must not contain directory separators.
  EXPECT_EQ(exe_name.find('/'), std::string::npos) << "Name should not contain '/'";
  EXPECT_EQ(exe_name.find('\\'), std::string::npos) << "Name should not contain '\\'";
}

TEST(ProcessDetectorUtilsTest, GetProcessCreationTimeTest)
{
  int32_t pid          = getpid();
  std::string iso_time = opentelemetry::resource_detector::detail::GetProcessCreationTime(pid);

#if defined(_MSC_VER) || defined(__linux__) || defined(__APPLE__)
  // On supported platforms we expect a non-empty ISO 8601 result.
  EXPECT_FALSE(iso_time.empty()) << "Creation time should be non-empty on this platform";

  if (!iso_time.empty())
  {
    // Very basic format check: "YYYY-MM-DDTHH:MM:SS.mmmZ" = 24 chars.
    EXPECT_GE(iso_time.size(), 20u) << "ISO 8601 string too short: " << iso_time;
    EXPECT_EQ(iso_time[4], '-') << "Expected '-' at index 4: " << iso_time;
    EXPECT_EQ(iso_time[7], '-') << "Expected '-' at index 7: " << iso_time;
    EXPECT_EQ(iso_time[10], 'T') << "Expected 'T' at index 10: " << iso_time;
    EXPECT_EQ(iso_time.back(), 'Z') << "Expected 'Z' at end: " << iso_time;
  }
#else
  // On unsupported platforms accept an empty string gracefully.
  (void)iso_time;
#endif
}

TEST(ProcessDetectorUtilsTest, GetProcessOwnerTest)
{
  int32_t pid       = getpid();
  std::string owner = opentelemetry::resource_detector::detail::GetProcessOwner();

  // On all supported platforms the effective user name must be non-empty.
  EXPECT_FALSE(owner.empty()) << "Process owner should be non-empty";
  // Sanity: no newline characters in the username.
  EXPECT_EQ(owner.find('\n'), std::string::npos) << "Owner should not contain newline";
}

TEST(ProcessDetectorUtilsTest, GetExecutableBuildIdHtlhashTest)
{
  int32_t pid          = getpid();
  std::string exe_path = opentelemetry::resource_detector::detail::GetExecutableInfo(pid).path;
  std::string hash1    = opentelemetry::resource_detector::detail::GetExecutableBuildIdHtlhash(pid);
  std::string hash2    = opentelemetry::resource_detector::detail::GetExecutableBuildIdHtlhash(pid);

  if (exe_path.empty())
  {
    EXPECT_TRUE(hash1.empty()) << "htlhash should be empty when executable path is empty";
    return;
  }

  EXPECT_FALSE(hash1.empty()) << "Build ID htlhash must not be empty";

  if (!hash1.empty())
  {
    // Must be exactly 32 lowercase hex characters (16 bytes).
    EXPECT_EQ(hash1.size(), 32u) << "htlhash must be 32 hex chars, got: " << hash1;
    for (char c : hash1)
    {
      EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
          << "Non-hex character '" << c << "' in htlhash: " << hash1;
    }
    // Must be deterministic: two calls on the same process → same result.
    EXPECT_EQ(hash1, hash2) << "htlhash must be deterministic";
  }
}

// ---------------------------------------------------------------------------
// Integration test — Detect() attribute presence
// ---------------------------------------------------------------------------

TEST(ProcessDetectorUtilsTest, ComputeSha256HexTest)
{
  // Test vectors from NIST FIPS 180-4
  // 1. Empty string
  EXPECT_EQ(opentelemetry::resource_detector::detail::ComputeSha256Hex(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

  // 2. "abc"
  EXPECT_EQ(opentelemetry::resource_detector::detail::ComputeSha256Hex("abc"),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

  // 3. "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  EXPECT_EQ(opentelemetry::resource_detector::detail::ComputeSha256Hex(
                "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST(ProcessResourceDetectorTest, DetectPopulatesExpectedAttributes)
{
  opentelemetry::resource_detector::ProcessResourceDetector detector;
  opentelemetry::sdk::resource::Resource resource = detector.Detect();
  const auto &attrs                               = resource.GetAttributes();

  // process.pid — always present.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessPid), attrs.end())
      << "process.pid must be present";

#if defined(_MSC_VER) || defined(__linux__)
  // process.executable.path — present on Linux and Windows.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessExecutablePath), attrs.end())
      << "process.executable.path must be present on this platform";

  // process.executable.name — present whenever executable.path is.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessExecutableName), attrs.end())
      << "process.executable.name must be present on this platform";

  // process.executable.build_id.htlhash — present on Linux and Windows.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessExecutableBuildIdHtlhash),
            attrs.end())
      << "process.executable.build_id.htlhash must be present on this platform";
#endif

#if defined(_MSC_VER) || defined(__linux__) || defined(__APPLE__)
  // process.creation.time — present on Linux, macOS, and Windows.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessCreationTime), attrs.end())
      << "process.creation.time must be present on this platform";

  // process.owner — present on Linux, macOS, and Windows.
  EXPECT_NE(attrs.find(opentelemetry::semconv::process::kProcessOwner), attrs.end())
      << "process.owner must be present on this platform";
#endif
}
