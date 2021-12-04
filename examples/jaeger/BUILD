cc_binary(
    name = "example_jaeger",
    srcs = [
        "main.cc",
    ],
    tags = ["jaeger"],
    deps = [
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/jaeger:opentelemetry_exporter_jaeger_trace",
        "//sdk/src/trace",
    ],
)
