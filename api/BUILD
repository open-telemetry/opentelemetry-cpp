# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@bazel_skylib//rules:common_settings.bzl", "int_flag", "string_flag")
load("//bazel:otel_cc.bzl", "otel_cc_library")

package(default_visibility = ["//visibility:public"])

CPP_STDLIBS = [
    "none",
    "best",
    "2014",
    "2017",
    "2020",
    "2023",
]

string_flag(
    name = "with_cxx_stdlib",
    build_setting_default = "best",
    values = CPP_STDLIBS,
)

otel_cc_library(
    name = "api",
    defines = ["HAVE_ABSEIL"] + select({
        ":set_cxx_stdlib_none": [],
        ### automatic selection
        ":set_cxx_stdlib_best": ["OPENTELEMETRY_STL_VERSION=(__cplusplus/100)"],
        # See https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
        ":set_cxx_stdlib_best_and_msvc": ["OPENTELEMETRY_STL_VERSION=(_MSVC_LANG/100)"],
        ### manual selection
        ":set_cxx_stdlib_2014": ["OPENTELEMETRY_STL_VERSION=2014"],
        ":set_cxx_stdlib_2017": ["OPENTELEMETRY_STL_VERSION=2017"],
        ":set_cxx_stdlib_2020": ["OPENTELEMETRY_STL_VERSION=2020"],
        ":set_cxx_stdlib_2023": ["OPENTELEMETRY_STL_VERSION=2023"],
        "//:with_dll_enabled": [],  # For select completeness
    }) + select({
        "//:with_dll_enabled": ["OPENTELEMETRY_DLL=-1"],
        "//conditions:default": ["OPENTELEMETRY_DLL=0"],
    }) + select({
        ":abi_version_no_1": ["OPENTELEMETRY_ABI_VERSION_NO=1"],
        ":abi_version_no_2": ["OPENTELEMETRY_ABI_VERSION_NO=2"],
    }),
    tags = ["api"],
    deps = [
        "headers",
        "@abseil-cpp//absl/base",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/types:variant",
    ],
)

otel_cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
)

# This is used for otel_sdk.zip packaging
filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)

[config_setting(
    name = "set_cxx_stdlib_%s" % v,
    flag_values = {
        ":with_cxx_stdlib": v,
        "//:with_dll": "false",
    },
) for v in CPP_STDLIBS]

config_setting(
    name = "set_cxx_stdlib_best_and_msvc",
    constraint_values = ["@bazel_tools//tools/cpp:msvc"],
    flag_values = {":with_cxx_stdlib": "best"},
)

int_flag(
    name = "abi_version_no",
    build_setting_default = 1,
)

config_setting(
    name = "abi_version_no_1",
    flag_values = {":abi_version_no": "1"},
)

config_setting(
    name = "abi_version_no_2",
    flag_values = {":abi_version_no": "2"},
)
