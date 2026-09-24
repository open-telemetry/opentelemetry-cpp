# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "w3c_tracecontext_http_test_server",
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
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:curl_implementation_headers",
        "//ext:headers",
        "//ext:server_headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)
