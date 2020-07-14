package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stream_span",
    srcs = [
        "stream.cc",
    ],
    hdrs = [
        "stream.h",
    ],
    deps = [
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "stream_span_test",
    srcs = ["stream_span_test.cc"],
    deps = [
        ":stream_span",
        "@com_google_googletest//:gtest_main",
    ],
)