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

// The following tests are based on the opentelemetry-java-instrumentation test cases for docker,
// podman, and cri-o mountinfo files:
// https://github.com/open-telemetry/opentelemetry-java-instrumentation/blob/3eafaa0539f1e38aae788d7c7fc60c74ad0d7290/instrumentation/resources/library/src/test/resources/podman_proc_self_mountinfo1
// https://github.com/open-telemetry/opentelemetry-java-instrumentation/blob/3eafaa0539f1e38aae788d7c7fc60c74ad0d7290/instrumentation/resources/library/src/test/resources/crio_proc_self_mountinfo
TEST(ContainerIdDetectorTest, ExtractValidContainerIdFromMountInfoLine)
{
  const std::string expected_id =
      "1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a";

  const char *lines[] = {
      // docker
      "9408 9397 259:5 "
      "/docker/containers/1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/"
      "resolv.conf /etc/resolv.conf rw,relatime - ext4 /dev/nvme1n1p3 rw",
      "9409 9397 259:5 "
      "/docker/containers/1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/"
      "hostname /etc/hostname rw,relatime - ext4 /dev/nvme1n1p3 rw",
      "9410 9397 259:5 "
      "/docker/containers/1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/"
      "hosts /etc/hosts rw,relatime - ext4 /dev/nvme1n1p3 rw",
      // podman rootless, root relative to the tmpfs mount (java: podman_proc_self_mountinfo1)
      "981 961 0:56 "
      "/containers/overlay-containers/"
      "1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/"
      "userdata/resolv.conf /etc/resolv.conf ro,nosuid,nodev,noexec,relatime - tmpfs tmpfs "
      "rw,size=783888k,nr_inodes=195972,mode=700,uid=2024,gid=2024,inode64",
      // podman / cri-o
      "1120 1118 0:42 /run/containers/storage/overlay-containers/"
      "1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/userdata/resolv.conf "
      "/etc/resolv.conf rw,nodev,relatime - ext4 /dev/sda1 rw",
      // cri-o application container secrets mount (java: crio_proc_self_mountinfo)
      "10316 10303 0:25 /containers/storage/overlay-containers/"
      "1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/userdata/run/secrets "
      "/run/secrets rw,nosuid,nodev - tmpfs tmpfs "
      "rw,seclabel,size=6416204k,nr_inodes=819200,mode=755,inode64",
  };

  for (const char *line : lines)
  {
    EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(line),
              expected_id)
        << line;
  }
}

// containerd mounts /etc/hostname from the pod sandbox, which is not the container id.
// This case is taken from the opentelemetry-java-instrumentation test:
// https://github.com/open-telemetry/opentelemetry-java-instrumentation/blob/3eafaa0539f1e38aae788d7c7fc60c74ad0d7290/instrumentation/resources/library/src/test/resources/containerd_proc_self_mountinfo
TEST(ContainerIdDetectorTest, DoesNotExtractSandboxIdFromContainerdMountInfoLine)
{
  std::string line =
      "2023 2002 253:1 /var/lib/containerd/io.containerd.grpc.v1.cri/sandboxes/"
      "b136f3d296b4c2024b3e7ad816f2a804a47cf1acc3d445075c6d78cf159ef58d/hostname /etc/hostname "
      "rw,relatime - xfs /dev/mapper/ubuntu--vg-root "
      "rw,attr2,inode64,logbufs=8,logbsize=32k,noquota";
  EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(line),
            std::string{""});
}

// The overlay line is taken from the opentelemetry-java-instrumentation test:
// https://github.com/open-telemetry/opentelemetry-java-instrumentation/blob/3eafaa0539f1e38aae788d7c7fc60c74ad0d7290/instrumentation/resources/library/src/test/resources/docker_proc_self_mountinfo
TEST(ContainerIdDetectorTest, DoesNotExtractIdFromMountInfoOptionsField)
{
  std::string line =
      "9408 9397 259:5 /tmp/random_mount /etc/resolv.conf rw,namespace=/docker/containers/"
      "1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a/resolv.conf - ext4 "
      "/dev/nvme1n1p3 rw";
  EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(line),
            std::string{""});

  // 64 hex overlay layer ids in lowerdir/upperdir must not be picked up either.
  std::string overlay =
      "456 375 0:143 / / rw,relatime master:175 - overlay overlay "
      "rw,lowerdir=/var/lib/docker/overlay2/l/CBPR2ETR4Z3UMOOGIIRDVT2P27,"
      "upperdir=/var/lib/docker/overlay2/"
      "3ef3e5a1a87b4e220c1da9a7901654e945b0ef5398e1b67fccb42fdb7750829e/diff";
  EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(overlay),
            std::string{""});
}

TEST(ContainerIdDetectorTest, DoesNotExtractNonHexOrWrongLengthFromMountInfoLine)
{
  std::string wrong_length =
      "9408 9397 259:5 /docker/containers/1aac2bcc27c4f/resolv.conf /etc/resolv.conf rw,relatime "
      "- ext4 /dev/nvme1n1p3 rw";
  EXPECT_EQ(
      opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(wrong_length),
      std::string{""});

  std::string too_long =
      "9408 9397 259:5 "
      "/docker/containers/1aac2bcc27c4f738c1a176cb71b47527fe73e26f80980a8f99275c93afe4d21a0/"
      "resolv.conf /etc/resolv.conf rw,relatime - ext4 /dev/nvme1n1p3 rw";
  EXPECT_EQ(opentelemetry::resource_detector::detail::ExtractContainerIDFromMountInfoLine(too_long),
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

// In Kubernetes /etc/hostname comes from the pod sandbox container while /run/secrets comes
// from the application container, whose id must be returned.
// This case is taken from the opentelemetry-java-instrumentation tests:
// https://github.com/open-telemetry/opentelemetry-java-instrumentation/blob/3eafaa0539f1e38aae788d7c7fc60c74ad0d7290/instrumentation/resources/library/src/test/resources/crio_proc_self_mountinfo
TEST(ContainerIdDetectorTest, ExtractLastIdFromKubernetesCrioMountInfoFile)
{
  const char *filename = "test_crio_mountinfo.txt";

  {
    std::ofstream outfile(filename);
    outfile << "10312 10303 0:25 /containers/storage/overlay-containers/"
               "2ac4c84cb0d3c3beb04beeef6ccf71c17b5fdd0252ce3a2b66bc2fdd0aaa1814/userdata/"
               "hostname /etc/hostname rw,nosuid,nodev master:15 - tmpfs tmpfs rw\n";
    outfile << "10316 10303 0:25 /containers/storage/overlay-containers/"
               "a8f62e52ed7c2cd85242dcf0eb1d727b643540ceca7f328ad7d2f31aedf07731/userdata/run/"
               "secrets /run/secrets rw,nosuid,nodev - tmpfs tmpfs rw\n";
  }

  std::string container_id =
      opentelemetry::resource_detector::detail::GetContainerIDFromMountInfo(filename);
  EXPECT_EQ(container_id,
            std::string{"a8f62e52ed7c2cd85242dcf0eb1d727b643540ceca7f328ad7d2f31aedf07731"});

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
