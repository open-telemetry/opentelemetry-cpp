# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "utf8_range",
    srcs = [
        "internal/utf8_range/uft8_range.cc",
    ],
    hdrs = [
        "internal/utf8_range/utf8_range.h",
        "internal/utf8_range/utf8_range_neon.inc",
        "internal/utf8_range/utf8_range_sse.inc",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
    ],
)

cc_library(
    name = "attribute_validity",
    srcs = [
        "attribute_validity.cc",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:global_log_handler",
        "//sdk/src/common:utf8_range",
    ],
)

cc_library(
    name = "random",
    srcs = [
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
    name = "base64",
    srcs = [
        "base64.cc",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common/platform:fork",
    ],
)

cc_library(
    name = "disabled",
    srcs = [
        "disabled.cc",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
        "//sdk:headers",
    ],
)

cc_library(
    name = "env_variables",
    srcs = [
        "env_variables.cc",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:global_log_handler",
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
