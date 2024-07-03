# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "zipkin_recordable",
    srcs = [
        "src/recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/zipkin/recordable.h",
    ],
    strip_include_prefix = "include",
    tags = ["zipkin"],
    deps = [
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@nlohmann_json//:json",
    ],
)

otel_cc_library(
    name = "zipkin_exporter",
    srcs = [
        "src/zipkin_exporter.cc",
        "src/zipkin_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/zipkin/zipkin_exporter.h",
        "include/opentelemetry/exporters/zipkin/zipkin_exporter_factory.h",
        "include/opentelemetry/exporters/zipkin/zipkin_exporter_options.h",
    ],
    copts = [
        "-DCURL_STATICLIB",
    ],
    strip_include_prefix = "include",
    tags = ["zipkin"],
    deps = [
        ":zipkin_recordable",
        "//ext/src/http/client/curl:http_client_curl",
    ],
)

otel_cc_test(
    name = "zipkin_recordable_test",
    srcs = ["test/zipkin_recordable_test.cc"],
    tags = [
        "test",
        "zipkin",
    ],
    deps = dll_deps([
        ":zipkin_recordable",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
    ]),
)

otel_cc_test(
    name = "zipkin_exporter_test",
    srcs = ["test/zipkin_exporter_test.cc"],
    tags = [
        "test",
        "zipkin",
    ],
    deps = dll_deps([
        ":zipkin_exporter",
        ":zipkin_recordable",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
