# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:wldap32.lib",
        ],
        "//conditions:default": [],
    }),
    tags = ["test"],
    deps = [
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
