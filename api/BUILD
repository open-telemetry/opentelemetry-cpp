# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

package(default_visibility = ["//visibility:public"])

bool_flag(
    name = "with_abseil",
    build_setting_default = False,
)

cc_library(
    name = "api",
    hdrs = glob(["include/**/*.h"]),
    defines = select({
        ":with_external_abseil": ["HAVE_ABSEIL", "OPENTELEMETRY_DLL=0"],
        "//:with_dll_enabled": ["OPENTELEMETRY_DLL=-1"], # -1=dllexport, 1=dllimport, 0=static
        "//conditions:default": ["OPENTELEMETRY_DLL=0"],
    }),
    strip_include_prefix = "include",
    tags = ["api"],
    deps = select({
        ":with_external_abseil": [
            "@com_google_absl//absl/base",
            "@com_google_absl//absl/strings",
            "@com_google_absl//absl/types:variant",
        ],
        "//conditions:default": [],
    }),
)

config_setting(
    name = "with_external_abseil",
    flag_values = {":with_abseil": "true"},
)

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
)

# This is used for otel_sdk.zip packaging
filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)
