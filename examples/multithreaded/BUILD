# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_binary(
    name = "example_multithreaded",
    srcs = [
        "main.cc",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]),
)
