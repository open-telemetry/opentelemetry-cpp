# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_binary(
    name = "example_logs_simple",
    srcs = [
        "main.cc",
    ],
    tags = [
        "examples",
        "ostream",
    ],
    deps = dll_deps([
        "//api",
        "//examples/common/logs_foo_library:common_logs_foo_library",
        "//exporters/ostream:ostream_log_record_exporter",
        "//exporters/ostream:ostream_span_exporter",
    ]),
)
