load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

otel_cc_benchmark(
    name = "span_id_benchmark",
    srcs = ["span_id_benchmark.cc"],
    deps = ["//api"],
)

[cc_test(
    name = f[:-3],
    srcs = [f],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
) for f in glob(["*_test.cc"])]
