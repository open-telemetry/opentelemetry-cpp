# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "http_server_test",
    srcs = [
        "http_server_test.cc",
    ],
    # The would-block test uses a POSIX socketpair; the body is compiled out on Windows.
    tags = ["test"],
    deps = [
        "//ext:headers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "socket_tools_test",
    srcs = [
        "socket_tools_test.cc",
    ],
    # ws2_32 is an interface linkopt of //ext:headers, so it is inherited here rather than
    # relinked, which also lets the test verify the public target's usage requirements.
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
