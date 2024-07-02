# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "es_log_record_exporter",
    srcs = [
        "src/es_log_record_exporter.cc",
        "src/es_log_recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/es_log_record_exporter.h",
        "include/opentelemetry/exporters/elasticsearch/es_log_recordable.h",
    ],
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

otel_cc_test(
    name = "es_log_record_exporter_test",
    srcs = ["test/es_log_record_exporter_test.cc"],
    tags = [
        "es",
        "test",
    ],
    deps = dll_deps([
        ":es_log_record_exporter",
        "@com_google_googletest//:gtest_main",
        "@curl",
        # This was not exported in the "dll" version, hence it needs to be linked in statically
        # In the non-dll version, it'll be just a duplicate dependency, and would be ignored        
        "@github_nlohmann_json//:json",
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
