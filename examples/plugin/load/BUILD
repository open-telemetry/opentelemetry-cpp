# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "load_plugin",
    srcs = [
        "main.cc",
    ],
    linkopts = [
        "-ldl",
    ],
    deps = [
        "//api",
    ],
)
