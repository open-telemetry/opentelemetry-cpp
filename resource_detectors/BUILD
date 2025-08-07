# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "resource_detectors",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["include/opentelemetry/resource_detectors/*.h"]),
    include_prefix = "opentelemetry/resource_detectors",
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/resource", 
    ],
)
