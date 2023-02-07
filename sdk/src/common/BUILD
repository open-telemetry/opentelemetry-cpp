# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "random",
    srcs = [
        "core.cc",
        "random.cc",
    ],
    hdrs = [
        "fast_random_number_generator.h",
        "random.h",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common/platform:fork",
    ],
)

cc_library(
    name = "global_log_handler",
    srcs = [
        "global_log_handler.cc",
    ],
    deps = [
        "//api",
        "//sdk:headers",
    ],
)
