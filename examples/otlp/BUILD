load("//:dll_deps.bzl", "dll_deps")

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
    deps = dll_deps([
        "//api",
        "//exporters/otlp:otlp_grpc_exporter",
        "//sdk/src/trace",
    ]) + [
        "//examples/common/foo_library:common_foo_library",
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
    deps = dll_deps([
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ]) + [
        "//examples/common/foo_library:common_foo_library",
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
    deps = dll_deps([
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//exporters/otlp:otlp_http_log_record_exporter",
        "//sdk/src/trace",
    ]) + [
        "//examples/common/logs_foo_library:common_logs_foo_library",
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
    deps = dll_deps([
        "//api",
        "//exporters/otlp:otlp_grpc_exporter",
        "//exporters/otlp:otlp_grpc_log_record_exporter",
        "//sdk/src/trace",
    ]) + [
        "//examples/common/logs_foo_library:common_logs_foo_library",
    ],
)

cc_binary(
    name = "example_otlp_grpc_metrics",
    srcs = [
        "grpc_metric_main.cc",
    ],
    tags = [
        "examples",
        "metrics",
        "otlp",
    ],
    deps = dll_deps([
        "//api",
        "//exporters/otlp:otlp_grpc_exporter",
        "//exporters/otlp:otlp_grpc_metric_exporter",
        "//sdk/src/metrics",
    ]) + [
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
    ],
)
