# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@googletest//:gtest_main",
        "@curl",
    ]),
)

otel_cc_test(
    name = "url_parser_test",
    srcs = [
        "url_parser_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext:headers",
        "//sdk/src/trace",
        "@googletest//:gtest_main",
    ]),
)
