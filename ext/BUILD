# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Public API: 4 stable headers forming the interface for HTTP clients
cc_library(
    name = "headers",
    hdrs = [
        "include/opentelemetry/ext/http/client/http_client.h",
        "include/opentelemetry/ext/http/client/http_client_factory.h",
        "include/opentelemetry/ext/http/client/curl/http_client_factory_curl.h",
        "include/opentelemetry/ext/http/common/url_parser.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
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
        "//ext/src/http/client/curl:__pkg__",
        "//exporters/elasticsearch:__pkg__",
        "//exporters/jaeger:__pkg__",
        "//exporters/otlp:__pkg__",
        "//exporters/zipkin:__pkg__",
    ],
    deps = [
        ":headers",
    ],
)
