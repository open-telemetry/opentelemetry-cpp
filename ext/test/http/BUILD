# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "socket_sigpipe_test",
    srcs = [
        "socket_sigpipe_test.cc",
    ],
    # socket_tools.h instantiates a WsaInitializer global on Windows (WSAStartup), so the Windows
    # build links ws2_32 even though the SIGPIPE test body itself is compiled out there.
    linkopts = select({
        "//bazel:windows": ["-DEFAULTLIB:Ws2_32.lib"],
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
