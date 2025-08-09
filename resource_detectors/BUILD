# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(
        ["include/**/*.h"],
        exclude = ["include/opentelemetry/resource_detectors/container_detector_utils.h"],
    ),
    strip_include_prefix = "include",
)

cc_library(
    name = "resource_detectors",
    srcs = [
        "container_detector.cc",
        "container_detector_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/resource_detectors/container_detector_utils.h",
    ],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)
