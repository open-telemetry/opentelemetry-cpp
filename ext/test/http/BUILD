# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "socket_tools_test",
    srcs = [
        "socket_tools_test.cc",
    ],
    linkopts = select({
        "//bazel:windows": ["-DEFAULTLIB:ws2_32.lib"],
        "//conditions:default": [],
    }),
    tags = ["test"],
    deps = [
        "//ext:headers",
        "@com_google_googletest//:gtest_main",
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
