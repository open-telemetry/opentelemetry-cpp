# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load( "//:dll_deps.bzl", "dll_deps" )

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    tags = [
        "examples",
        "ostream",
    ],
    deps = dll_deps([
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/ostream:ostream_span_exporter",
    ]),
)
