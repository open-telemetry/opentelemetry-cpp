# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_test")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "es_log_record_exporter",
    srcs = [
        "src/es_log_record_exporter.cc",
        "src/es_log_recordable.cc",
    ],
    tags = ["es"],
    deps = [
        "headers",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/logs",
        "@nlohmann_json//:json",
    ],
)

otel_cc_test(
    name = "es_log_record_exporter_test",
    srcs = ["test/es_log_record_exporter_test.cc"],
    tags = [
        "es",
        "test",
    ],
    deps = dll_deps([
        ":es_log_record_exporter",
        "@googletest//:gtest_main",
        # This was not exported in the "dll" version, hence it needs to be linked in statically
        # In the non-dll version, it'll be just a duplicate dependency, and would be ignored
        "@nlohmann_json//:json",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)

otel_cc_library(
    name = "headers",
    hdrs = [":header_files"],
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

