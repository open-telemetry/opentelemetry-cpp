# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "resource",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "src/resource",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:env_variables",
    ],
)
