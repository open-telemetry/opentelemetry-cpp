# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "container_resource_detector_test",
    srcs = ["container_detector_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:container_resource_detector",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "env_entity_resource_detector_test",
    srcs = ["env_entity_detector_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:env_entity_resource_detector",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "host_resource_detector_test",
    srcs = ["host_detector_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:host_resource_detector",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "process_resource_detector_test",
    srcs = ["process_detector_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:process_resource_detector",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

# Kept separate from detector tests: linking configuration_core pulls in
# sdk_builder.cc, which requires exception support, so these targets are excluded
# from the bazel.noexcept build in ci/do_ci.sh.
cc_test(
    name = "container_resource_detector_builder_test",
    srcs = ["container_detector_builder_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:container_resource_detector_builder",
        "//sdk/src/configuration:configuration_core",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "process_resource_detector_builder_test",
    srcs = ["process_detector_builder_test.cc"],
    tags = ["test"],
    deps = [
        "//resource_detectors:process_resource_detector_builder",
        "//sdk/src/configuration:configuration_core",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)
