# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
)

cc_library(
    name = "container_resource_detector",
    srcs = [
        "src/container_detector.cc",
        "src/container_detector_utils.cc",
    ],
    copts = ["-fexceptions"],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)

cc_library(
    name = "container_resource_detector_builder",
    srcs = ["src/container_detector_builder.cc"],
    deps = [
        ":container_resource_detector",
        "//sdk/src/configuration:configuration_core",
    ],
)

cc_library(
    name = "env_entity_resource_detector",
    srcs = ["src/env_entity_detector.cc"],
    copts = ["-fexceptions"],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)

cc_library(
    name = "host_resource_detector",
    srcs = [
        "src/host_detector.cc",
        "src/host_detector_utils.cc",
    ],
    copts = ["-fexceptions"],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)

cc_library(
    name = "process_resource_detector",
    srcs = [
        "src/process_detector.cc",
        "src/process_detector_utils.cc",
    ],
    copts = ["-fexceptions"],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)

cc_library(
    name = "process_resource_detector_builder",
    srcs = ["src/process_detector_builder.cc"],
    deps = [
        ":process_resource_detector",
        "//sdk/src/configuration:configuration_core",
    ],
)

# Convenience target linking all resource detectors.
cc_library(
    name = "resource_detectors",
    deps = [
        ":container_resource_detector",
        ":env_entity_resource_detector",
        ":host_resource_detector",
        ":process_resource_detector",
    ],
)

# Convenience target linking all resource detector builders.
cc_library(
    name = "resource_detectors_builders",
    deps = [
        ":container_resource_detector_builder",
        ":process_resource_detector_builder",
    ],
)
