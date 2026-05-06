# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "example_etw_threads",
    srcs = [
        "main.cc",
    ],
    tags = [
        "etw",
        "examples",
    ],
    target_compatible_with = ["@platforms//os:windows"],
    deps = [
        "//api",
        "//exporters/etw:etw_exporter",
    ],
)
