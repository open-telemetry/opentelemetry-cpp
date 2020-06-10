load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "context_test",
    srcs = [
        "context_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "runtimeContext_test",
    srcs = [
        "runtimeContext_test.cc",
    ],
    copts = ["-Wall","-std=c++17"],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

