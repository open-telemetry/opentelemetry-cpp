// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/detail/host_detector_utils.h"
#include "opentelemetry/resource_detectors/host_detector.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/incubating/host_attributes.h"
#include "opentelemetry/semconv/schema_url.h"

namespace semconv = opentelemetry::semconv;

TEST(HostDetectorUtilsTest, GetHostName)
{
  std::string host_name = opentelemetry::resource_detector::detail::GetHostName();
  EXPECT_FALSE(host_name.empty());
}

TEST(HostDetectorUtilsTest, GetHostArch)
{
  std::string arch = opentelemetry::resource_detector::detail::GetHostArch();

#if defined(_M_AMD64) || defined(__x86_64__)
  EXPECT_EQ(arch, std::string{semconv::host::HostArchValues::kAmd64});
#elif defined(_M_ARM64) || defined(__aarch64__)
  EXPECT_EQ(arch, std::string{semconv::host::HostArchValues::kArm64});
#endif

  if (!arch.empty())
  {
    const char *known_values[] = {
        semconv::host::HostArchValues::kAmd64, semconv::host::HostArchValues::kArm32,
        semconv::host::HostArchValues::kArm64, semconv::host::HostArchValues::kIa64,
        semconv::host::HostArchValues::kPpc32, semconv::host::HostArchValues::kPpc64,
        semconv::host::HostArchValues::kS390x, semconv::host::HostArchValues::kX86};
    bool found = false;
    for (const char *value : known_values)
    {
      if (arch == value)
      {
        found = true;
        break;
      }
    }
    EXPECT_TRUE(found);
  }
}

TEST(HostDetectorUtilsTest, GetHostIdFromFile)
{
  const char *filename = "test_machine_id.txt";

  {
    std::ofstream outfile(filename);
    outfile << "0f81dc7e29c2446fb0f232fe8f42e027\n";
  }

  std::string host_id = opentelemetry::resource_detector::detail::GetHostIdFromFile(filename);
  EXPECT_EQ(host_id, std::string{"0f81dc7e29c2446fb0f232fe8f42e027"});

  std::remove(filename);
}

TEST(HostDetectorUtilsTest, GetHostIdFromEmptyFile)
{
  const char *filename = "test_empty_machine_id.txt";

  {
    std::ofstream outfile(filename);
  }

  std::string host_id = opentelemetry::resource_detector::detail::GetHostIdFromFile(filename);
  EXPECT_TRUE(host_id.empty());

  std::remove(filename);
}

TEST(HostDetectorUtilsTest, GetHostIdFromMissingFile)
{
  std::string host_id =
      opentelemetry::resource_detector::detail::GetHostIdFromFile("test_missing_machine_id.txt");
  EXPECT_TRUE(host_id.empty());
}

TEST(HostDetectorTest, DetectSetsHostName)
{
  opentelemetry::resource_detector::HostResourceDetector detector;
  auto resource     = detector.Detect();
  const auto &attrs = resource.GetAttributes();

  auto it = attrs.find(semconv::host::kHostName);
  ASSERT_NE(it, attrs.end());
  EXPECT_FALSE(opentelemetry::nostd::get<std::string>(it->second).empty());

  EXPECT_EQ(resource.GetSchemaURL(), std::string{semconv::kSchemaUrl});
}
