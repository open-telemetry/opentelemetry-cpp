# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_binary(
    name = "example_multi_processor",
    srcs = [
        "main.cc",
    ],
    tags = [
        "memory",
        "ostream",
    ],
    deps = dll_deps([
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]),
)
