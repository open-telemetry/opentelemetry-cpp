# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "metrics",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "src/metrics",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:global_log_handler",
        "//sdk/src/resource",
    ],
)
