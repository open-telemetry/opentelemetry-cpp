// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>

#include "opentelemetry/resource_detectors/detail/container_detector_utils.h"

TEST(ContainerIdDetectorTest, ExtractValidContainerIdFromLine)
{
  std::string line =
      "13:name=systemd:/podruntime/docker/kubepods/ac679f8a8319c8cf7d38e1adf263bc08d23.aaaa";
  std::string extracted_id =
      opentelemetry::resource_detector::detail::ExtractContainerIDFromLine(line);
  EXPECT_EQ(std::string{"ac679f8a8319c8cf7d38e1adf263bc08d23"}, extracted_id);
}

TEST(ContainerIdDetectorTest, ExtractIdFromMockUpCGroupFile)
{
  const char *filename = "test_cgroup.txt";

  {
    std::ofstream outfile(filename);
    outfile << "13:name=systemd:/kuberuntime/containerd"
               "/kubepods-pod872d2066_00ef_48ea_a7d8_51b18b72d739:cri-containerd:"
               "e857a4bf05a69080a759574949d7a0e69572e27647800fa7faff6a05a8332aa1\n";
    outfile << "9:cpu:/not-a-container\n";
  }

  std::string container_id =
      opentelemetry::resource_detector::detail::GetContainerIDFromCgroup(filename);
  EXPECT_EQ(container_id,
            std::string{"e857a4bf05a69080a759574949d7a0e69572e27647800fa7faff6a05a8332aa1"});

  std::remove(filename);
}

TEST(ContainerIdDetectorTest, DoesNotExtractInvalidLine)
{
  std::string line = "this line does not contain a container id";
  std::string id   = opentelemetry::resource_detector::detail::ExtractContainerIDFromLine(line);
  EXPECT_EQ(id, std::string{""});
}

TEST(ContainerIdDetectorTest, ReturnsEmptyOnNoMatch)
{
  const char *filename = "test_empty_cgroup.txt";

  {
    std::ofstream outfile(filename);
    outfile << "no container id here\n";
  }

  std::string id = opentelemetry::resource_detector::detail::GetContainerIDFromCgroup(filename);
  EXPECT_EQ(id, std::string{""});

  std::remove(filename);  // cleanup
}

TEST(ContainerIdDetectorTest, ReturnsEmptyOnFileFailingToOpen)
{
  const char *filename = "test_invalid_cgroup.txt";

  std::string id = opentelemetry::resource_detector::detail::GetContainerIDFromCgroup(filename);
  EXPECT_EQ(id, std::string{""});
}

TEST(ContainerIdDetectorTest, ExtractValidContainerIdFromMountInfoLine)
{
  std::string line =
      "9266 6084 259:5 "
      "/docker/containers/e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592/"
      "hostname /etc/hostname rw,relatime - ext4 /dev/nvme1n1p3 rw";
  std::string extracted_id =
      opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(line);
  EXPECT_EQ(std::string{"e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592"},
            extracted_id);
}

TEST(ContainerIdDetectorTest, DoesNotExtractFromMountInfoLineWithoutKeywords)
{
  std::string line =
      "8248 6084 259:5 "
      "/docker/other/e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592/"
      "resolv.conf /etc/resolv.conf rw,relatime - ext4 /dev/nvme1n1p3 rw";
  std::string id =
      opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(line);
  EXPECT_EQ(id, std::string{""});
}

TEST(ContainerIdDetectorTest, DoesNotExtractNonHexOrWrongLengthFromMountInfoLine)
{
  std::string wrong_length =
      "9266 6084 259:5 /docker/containers/abcdef0123456789/hostname /etc/hostname rw - ext4 "
      "/dev/sda1 rw";
  EXPECT_EQ(
      opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(wrong_length),
      std::string{""});

  std::string non_hex =
      "9266 6084 259:5 "
      "/docker/containers/g9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592/"
      "hostname /etc/hostname rw - ext4 /dev/sda1 rw";
  EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(non_hex),
            std::string{""});
}

TEST(ContainerIdDetectorTest, ExtractIdFromMockUpMountInfoFile)
{
  const char *filename = "test_mountinfo.txt";

  {
    std::ofstream outfile(filename);
    outfile << "6084 6083 0:100 / / rw,relatime - overlay overlay rw,lowerdir=/var/lib/docker\n";
    outfile
        << "8248 6084 259:5 "
           "/docker/containers/e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592/"
           "resolv.conf /etc/resolv.conf rw,relatime - ext4 /dev/nvme1n1p3 rw\n";
    outfile
        << "9266 6084 259:5 "
           "/docker/containers/e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592/"
           "hostname /etc/hostname rw,relatime - ext4 /dev/nvme1n1p3 rw\n";
  }

  std::string container_id =
      opentelemetry::resource_detector::detail::GetContainerIDFromMountInfo(filename);
  EXPECT_EQ(container_id,
            std::string{"e9974a495c2e01d17b9c71d4469cd6636ca733cd514e6ee49e1435fc03a93592"});

  std::remove(filename);
}

TEST(ContainerIdDetectorTest, ReturnsEmptyOnMountInfoNoMatch)
{
  const char *filename = "test_empty_mountinfo.txt";

  {
    std::ofstream outfile(filename);
    outfile << "6084 6083 0:100 / / rw,relatime - overlay overlay rw\n";
  }

  std::string id = opentelemetry::resource_detector::detail::GetContainerIDFromMountInfo(filename);
  EXPECT_EQ(id, std::string{""});

  std::remove(filename);
}

TEST(ContainerIdDetectorTest, ReturnsEmptyOnMountInfoFileFailingToOpen)
{
  const char *filename = "test_invalid_mountinfo.txt";

  std::string id = opentelemetry::resource_detector::detail::GetContainerIDFromMountInfo(filename);
  EXPECT_EQ(id, std::string{""});
}
