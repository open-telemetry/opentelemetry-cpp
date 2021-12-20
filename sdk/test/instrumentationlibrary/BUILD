load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "instrumentationlibrary_test",
    srcs = [
        "instrumentationlibrary_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//api",
        "//sdk:headers",
        "@com_google_googletest//:gtest_main",
    ],
)
