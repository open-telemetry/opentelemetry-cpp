// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>

#include "opentelemetry/resource_detectors/process_detector_utils.h"

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

TEST(ProcessDetectorUtilsTest, ExtractCommandLineArgs)
{
  std::string filename{"test_args.txt"};

  {
    std::ofstream outfile(filename, std::ios::binary);
    const char raw_data[] = "test_command\0arg1\0arg2\0arg3\0";
    outfile.write(raw_data, sizeof(raw_data) - 1);
  }

  std::vector<std::string> args =
      opentelemetry::resource_detector::detail::ExtractCommandLineArgs(filename);
  EXPECT_EQ(args.size(), 4);
  EXPECT_EQ(args[0], "test_command");
  EXPECT_EQ(args[1], "arg1");
  EXPECT_EQ(args[2], "arg2");
  EXPECT_EQ(args[3], "arg3");

  std::remove(filename.c_str());  // Cleanup
}
