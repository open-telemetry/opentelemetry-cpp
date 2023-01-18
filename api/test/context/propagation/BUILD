load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps")

cc_test(
    name = "composite_propagator_test",
    srcs = [
        "composite_propagator_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = dll_deps([
        "//api",
    ]) + [
        "@com_google_googletest//:gtest_main",
    ],
)
