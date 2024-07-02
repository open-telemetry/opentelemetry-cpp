# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")
 
avoid_dll_lock()

otel_cc_binary(
    name = "example_http_client",
    srcs = [
        "client.cc",
        "tracer_common.h",
    ],
    copts = [
        "-DWITH_CURL",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
        ],
        "//conditions:default": [],
    }),
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
    ]),
)

otel_cc_binary(
    name = "example_http_server",
    srcs = [
        "server.cc",
        "server.h",
        "tracer_common.h",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//sdk/src/trace",
    ]),
)
