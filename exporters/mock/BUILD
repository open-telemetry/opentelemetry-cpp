package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mock_span_exporter",
    srcs = [
        'src/mock_span_exporter.cc',
    ],
    hdrs = [
        'include/opentelemetry/exporters/mock/mock_span_exporter.h',
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/trace",
    ],
)