# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "example_multi_processor",
    srcs = [
        "main.cc",
    ],
    tags = [
        "memory",
        "ostream",
        "tested_example",
    ],
    deps = [
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
