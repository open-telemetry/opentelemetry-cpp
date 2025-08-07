# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
)

cc_library(
    name = "resource_detectors",
    srcs = [
        "container.cc",
        "container_detector.cc",
    ],
    deps = [
        "//api",
        "//resource_detectors:headers",
        "//sdk:headers",
        "//sdk/src/resource",
    ],
)
