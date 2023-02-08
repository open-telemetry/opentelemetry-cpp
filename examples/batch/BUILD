# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load( "//:dll_deps.bzl", "dll_deps" )

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-lpthread"],
    tags = ["ostream"],
    deps = dll_deps([   
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]),
)
