# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "example_explicit_parent",
    srcs = [
        "main.cc",
    ],
    tags = ["ostream"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
