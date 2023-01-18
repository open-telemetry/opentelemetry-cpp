load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_binary(
    name = "context_test",
    srcs = [
        "context_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "runtime_context_test",
    srcs = [
        "runtime_context_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
