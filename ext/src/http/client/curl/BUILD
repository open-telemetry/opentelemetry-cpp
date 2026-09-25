# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

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
        "//ext:curl_implementation_headers",
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
