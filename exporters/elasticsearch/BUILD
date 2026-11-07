# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

# The bulk response parser is an implementation detail that the CMake package
# also leaves out. hdrs is a target's public interface, so it is kept in its own
# target that only this package can depend on, and reached through
# implementation_deps so the exporter does not re-export it.
cc_library(
    name = "es_bulk_response",
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/detail/es_bulk_response.h",
    ],
    strip_include_prefix = "include",
    tags = ["es"],
    visibility = ["//exporters/elasticsearch:__pkg__"],
    deps = [
        "//api",
        "@github_nlohmann_json//:json",
    ],
)

cc_library(
    name = "es_log_record_exporter",
    srcs = [
        "src/es_log_record_exporter.cc",
        "src/es_log_recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/es_log_record_exporter.h",
        "include/opentelemetry/exporters/elasticsearch/es_log_recordable.h",
    ],
    implementation_deps = [":es_bulk_response"],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    tags = ["es"],
    deps = [
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/logs",
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "es_log_record_exporter_test",
    srcs = ["test/es_log_record_exporter_test.cc"],
    tags = [
        "es",
        "test",
    ],
    deps = [
        ":es_bulk_response",
        ":es_log_record_exporter",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
