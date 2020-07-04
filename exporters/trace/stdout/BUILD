cc_library(
    name = "stdout_span",
    srcs = [
        "stdout.cc",
    ],
    hdrs = [
        "stdout.h",
    ],
    include_prefix = "exporters/trace/stdout",
    deps = [
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "stdout_span_test",
    srcs = ["stdout_span_test.cc"],
    deps = [
        ":stdout_span",
        "@com_google_googletest//:gtest_main",
    ],
)