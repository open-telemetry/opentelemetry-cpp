cc_binary(
    name = "e2e_example_otlp_grpc",
    srcs = [
        "grpc_main.cc",
    ],
    deps = [
        "@io_opentelemetry_cpp//api",
        "@io_opentelemetry_cpp//examples/common/foo_library:common_foo_library",
        "@io_opentelemetry_cpp//exporters/otlp:otlp_grpc_exporter",
        "@io_opentelemetry_cpp//sdk/src/trace",
    ],
)
