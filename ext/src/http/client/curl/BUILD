# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Internal implementation headers for curl HTTP client
# Used by: curl client implementation, internal tests, w3c test server
# These are implementation details that should not be depended on by external code
cc_library(
    name = "implementation_headers",
    hdrs = [
        "http_client_curl.h",
        "http_operation_curl.h",
        "http_time_util.h",
    ],
    strip_include_prefix = ".",
    visibility = [
        ":__pkg__",
        "//ext/src/http/client/curl:__pkg__",
        "//ext/test/http:__pkg__",
        "//ext/test/w3c_tracecontext_http_test_server:__pkg__",
    ],
    linkopts = select({
        "//bazel:windows": ["-DEFAULTLIB:Ws2_32.lib"],
        "//conditions:default": [],
    }),
    deps = [
        "//ext:headers",
        "@curl//:curl",
    ],
)

cc_library(
    name = "http_client_curl",
    srcs = [
        "http_client_curl.cc",
        "http_client_factory_curl.cc",
        "http_operation_curl.cc",
        "http_time_util.cc",
    ],
    defines = [
        "ENABLE_HTTP_CLIENT_CURL",
        "ENABLE_OTLP_RETRY_PREVIEW",
    ],
    include_prefix = "src/http/client/curl",
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
            "-DEFAULTLIB:Ws2_32.lib",
        ],
        "//conditions:default": [],
    }),
    linkstatic = True,
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//sdk/src/common:random",
        "@curl",
        "@zlib",
    ],
    # Implementation headers are not exposed in the public API, so they are
    # declared as implementation_deps to prevent transitive dependents from
    # accessing curl internals. Only the .cc files (via srcs) see these headers.
    implementation_deps = [
        ":implementation_headers",
    ],
)
