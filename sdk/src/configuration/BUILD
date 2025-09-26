# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "configuration",
    srcs = glob(["**/*.cc"]),
    include_prefix = "src/configuration",
    deps = [
        "//api",
        "//sdk:headers",
        "@rapidyaml",
    ],
)
