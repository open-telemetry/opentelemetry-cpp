load("//bazel:cc_static_library.bzl", "cc_static_library")

cc_binary(
    name = "example_otlp_grpc",
    srcs = [
        "grpc_main.cc",
    ],
    tags = [
        "examples",
        "otlp",
        "otlp_grpc",
    ],
    deps = [
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/otlp:otlp_grpc_exporter",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_otlp_http",
    srcs = [
        "http_main.cc",
    ],
    tags = [
        "examples",
        "otlp",
        "otlp_http",
    ],
    deps = [
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_otlp_http_log",
    srcs = [
        "http_log_main.cc",
    ],
    tags = [
        "examples",
        "otlp",
        "otlp_http_log",
    ],
    deps = [
        "//api",
        "//examples/common/logs_foo_library:common_logs_foo_library",
        "//exporters/otlp:otlp_http_exporter",
        "//exporters/otlp:otlp_http_log_exporter",
        "//sdk/src/trace",
    ],
)

# This only works on linux, and depends on a a custom rule added in
# bazel/cc_static_library.bzl and copied from:
# https://github.com/bazelbuild/bazel/issues/1920#issuecomment-1019661738
cc_static_library(
    name = "libdummy_remove_me",
    deps = [
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ],
)

# This is like example_otlp_http above, except it only depends on
# libaries that we actually want to package up. After building this
# on mac and windows, then we can search for .a/.lib files in the
# bazel-out directory.
cc_binary(
    name = "hw",
    linkstatic = True,
    srcs = [ "hw.cc" ],
    deps = [
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_otlp_grpc_log",
    srcs = [
        "grpc_log_main.cc",
    ],
    tags = [
        "examples",
        "otlp",
        "otlp_grpc_log",
    ],
    deps = [
        "//api",
        "//examples/common/logs_foo_library:common_logs_foo_library",
        "//exporters/otlp:otlp_grpc_exporter",
        "//exporters/otlp:otlp_grpc_log_exporter",
        "//sdk/src/trace",
    ],
)
