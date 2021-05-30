cc_library(
    name = "foo_library",
    srcs = [
        "foo_library/foo_library.cc",
    ],
    hdrs = [
        "foo_library/foo_library.h",
    ],
    deps = [
        "//api",
    ],
)

cc_binary(
    name = "example_otlp_grpc",
    srcs = [
        "grpc_main.cc",
    ],
    deps = [
        ":foo_library",
        "//api",
        "//exporters/otlp:otlp_exporter",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_otlp_http",
    srcs = [
        "http_main.cc",
    ],
    deps = [
        ":foo_library",
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/trace",
    ],
)
