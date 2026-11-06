# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "etw_exporter",
    hdrs = glob([
        "include/opentelemetry/exporters/etw/*.h",
    ]),
    includes = ["include"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = ["etw"],
    deps = [
        "//api",
        "//sdk/src/trace",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "etw_provider_test",
    srcs = ["test/etw_provider_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "etw_tracer_test",
    srcs = ["test/etw_tracer_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "etw_logger_test",
    srcs = ["test/etw_logger_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "etw_tracer_tld_test",
    srcs = ["test/etw_tracer_tld_test.cc"],
    tags = [
        "etw",
        "test",
    ],
    target_compatible_with = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "etw_tracer_tld_end_time_test",
    srcs = ["test/etw_tracer_tld_test.cc"],
    local_defines = ['ETW_FIELD_ENDTTIME=\\"EndTime\\"'],
    tags = [
        "etw",
        "test",
    ],
    target_compatible_with = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "etw_tracer_tld_custom_fields_test",
    srcs = [
        "test/custom_etw_fields.h",
        "test/etw_tracer_tld_test.cc",
    ],
    includes = ["test"],
    local_defines = ['CUSTOM_ETW_FIELDS_H=\\"custom_etw_fields.h\\"'],
    tags = [
        "etw",
        "test",
    ],
    target_compatible_with = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "etw_tracer_msgpack_test",
    srcs = ["test/etw_tracer_msgpack_test.cc"],
    local_defines = [
        "HAVE_FIELD_TIME",
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    target_compatible_with = select({
        "@platforms//os:windows": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":etw_exporter",
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
    ],
)
