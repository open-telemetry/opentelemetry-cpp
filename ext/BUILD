# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Public API: 4 stable headers forming the interface for HTTP clients
cc_library(
    name = "headers",
    hdrs = [
        "include/opentelemetry/ext/http/client/curl/http_client_factory_curl.h",
        "include/opentelemetry/ext/http/client/http_client.h",
        "include/opentelemetry/ext/http/client/http_client_factory.h",
        "include/opentelemetry/ext/http/common/url_parser.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
    ],
)

# Internal: curl implementation headers
# Used by: the curl client itself, its tests, and the w3c test server
# These are implementation details and are not installed by CMake
cc_library(
    name = "curl_implementation_headers",
    hdrs = [
        "include/opentelemetry/ext/http/client/curl/http_client_curl.h",
        "include/opentelemetry/ext/http/client/curl/http_operation_curl.h",
        "include/opentelemetry/ext/http/client/curl/http_time_util.h",
    ],
    strip_include_prefix = "include",
    visibility = [
        "//ext/src/http/client/curl:__pkg__",
        "//ext/test/http:__pkg__",
        "//ext/test/w3c_tracecontext_http_test_server:__pkg__",
    ],
    deps = [
        ":headers",
        "@curl",
    ],
)

# Internal: Detail headers for factory implementations
# Used by: exporters/otlp, exporters/elasticsearch, exporters/zipkin
# Breaking change in v2.0.0: moved from public to internal target
cc_library(
    name = "http_client_detail",
    hdrs = [
        "include/opentelemetry/ext/http/client/detail/default_factory.h",
    ],
    strip_include_prefix = "include",
    visibility = [
        "//exporters/elasticsearch:__pkg__",
        "//exporters/otlp:__pkg__",
        "//exporters/zipkin:__pkg__",
        "//ext/src/http/client/curl:__pkg__",
    ],
    deps = [
        ":headers",
    ],
)

# Internal: Embedded HTTP server headers (test and example use only)
# TODO: Move to test_common per issue #4332
# These are test infrastructure, not part of the public API
cc_library(
    name = "server_headers",
    hdrs = [
        "include/opentelemetry/ext/http/server/http_server.h",
        "include/opentelemetry/ext/http/server/socket_tools.h",
    ],
    linkopts = select({
        "//bazel:windows": ["-DEFAULTLIB:Ws2_32.lib"],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    visibility = [
        "//examples/http:__pkg__",
        "//exporters/otlp:__pkg__",
        "//ext/test/http:__pkg__",
        "//ext/test/w3c_tracecontext_http_test_server:__pkg__",
    ],
    deps = [
        "//api",
    ],
)
