package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stdout",
    srcs = [
        "stdout.cc",
    ],
    hdrs = [
        "stdout.h",
    ],
    deps = [
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "stdout_span_test",
    srcs = ["stdout_span_test.cc"],
    deps = [
        ":stdout",
        "@com_google_googletest//:gtest_main",
    ],
)