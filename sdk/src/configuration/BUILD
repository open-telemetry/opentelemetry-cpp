# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "configuration_core",
    srcs = [
        "composable_always_off_sampler_configuration.cc",
        "composable_always_on_sampler_configuration.cc",
        "composable_parent_threshold_sampler_configuration.cc",
        "composable_probability_sampler_configuration.cc",
        "composable_rule_based_sampler_configuration.cc",
        "configured_sdk.cc",
        "document_node.cc",
        "registry.cc",
        "sdk_builder.cc",
    ],
    include_prefix = "src/configuration",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:wildcard_match",
    ],
)

cc_library(
    name = "configuration",
    srcs = [
        "configuration_parser.cc",
        "ryml_document.cc",
        "ryml_document_node.cc",
        "yaml_configuration_parser.cc",
    ],
    include_prefix = "src/configuration",
    deps = [
        ":configuration_core",
        "//api",
        "//sdk:headers",
        "@rapidyaml",
    ],
)
