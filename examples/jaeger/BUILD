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
    name = "example_otlp",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":foo_library",
        "//api",
        "//exporters/jaeger:jaeger_exporter",
        "//sdk/src/trace",
    ],
)
