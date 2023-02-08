# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "w3c_tracecontext_test",
    srcs = [
        "main.cc",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
        ],
        "//conditions:default": [],
    }),
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
    ]) + [
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)
