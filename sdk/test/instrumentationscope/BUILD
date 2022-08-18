load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "instrumentationscope_test",
    srcs = [
        "instrumentationscope_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//api",
        "//sdk:headers",
        "@com_google_googletest//:gtest_main",
    ],
)
