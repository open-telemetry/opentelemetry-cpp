# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "resource_detector_test",
    srcs = [
        "container_detector_test.cc",
        "env_entity_detector_test.cc",
        "process_detector_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//api",
        "//resource_detectors",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

# Kept separate from resource_detector_test: linking configuration_core pulls in
# sdk_builder.cc, which requires exception support, so this target is excluded
# from the bazel.noexcept build in ci/do_ci.sh.
cc_test(
    name = "resource_detector_builder_test",
    srcs = [
        "container_detector_builder_test.cc",
        "process_detector_builder_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//api",
        "//resource_detectors",
        "//resource_detectors:resource_detectors_builders",
        "//sdk/src/configuration:configuration_core",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)
