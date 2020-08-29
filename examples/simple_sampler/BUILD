load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "test_tracer",
    srcs = [
        "test_tracer/test_tracer.cc",
    ],
    hdrs = [
        "test_tracer/test_tracer.h",
    ],
    deps = [
        "//api",
    ],
)

cc_library(
    name = "stdout_exporter",
    srcs = [
        "stdout_exporter.cc",
    ],
    hdrs = [
        "stdout_exporter.h",
    ],
    deps = [
        "//api",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":stdout_exporter",
        ":test_tracer",
        "//api",
        "//sdk/src/trace",
    ],
)
