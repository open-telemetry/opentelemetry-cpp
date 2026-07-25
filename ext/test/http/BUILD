# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

# file_http_server.h is an installed header no other target includes, so it was never compiled.
# This library compiles it (the POSIX branch here, the Windows branch on the Windows runners) so
# a build regression in it is caught.
cc_library(
    name = "file_http_server_header_compile",
    srcs = ["file_http_server_header_compile.cc"],
    tags = ["test"],
    deps = [
        "//ext:headers",
    ],
)

cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    defines = ["ENABLE_OTLP_RETRY_PREVIEW"],
    tags = ["test"],
    deps = [
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
