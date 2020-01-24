cc_test(
    name = "noop_test",
    srcs = [
        "noop_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "span_id_benchmark",
    srcs = [
        "span_id_benchmark.cc",
    ],
    deps = [
        "//api",
        "@com_github_google_benchmark//:benchmark",
    ],
)

cc_test(
    name = "span_id_test",
    srcs = [
        "span_id_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "trace_id_test",
    srcs = [
        "trace_id_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
