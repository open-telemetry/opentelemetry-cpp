# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ]),
)

cc_test(
    name = "url_parser_test",
    srcs = [
        "url_parser_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext:headers",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ]),
)
