cc_binary(
    name = "example_otlp_grpc",
    srcs = [
        "grpc_main.cc",
    ],
    deps = [
        "//examples/common/foo_library:common_foo_library",
        "//api",
        "//exporters/otlp:otlp_grpc_exporter",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_otlp_http",
    srcs = [
        "http_main.cc",
    ],
    deps = [
        "//examples/common/foo_library:common_foo_library",
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ],
)
