package(default_visibility = ["//visibility:public"])

cc_library(
    name = "etw_provider_exporter",
    srcs = [
        "src/etw_provider_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/etw/etw_provider_exporter.h",
        "include/opentelemetry/exporters/etw/utils.h",
        "include/opentelemetry/exporters/etw/uuid.hpp",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "etw_provider_test",
    srcs = ["test/etw_provider_test.cc"],
    deps = [
        ":etw_provider_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "etw_tracer_exporter",
    srcs = [
        "src/etw_tracer_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/etw/etw_tracer_exporter.h",
        "include/opentelemetry/exporters/etw/utils.h",
        "include/opentelemetry/exporters/etw/uuid.hpp",
    ],
    strip_include_prefix = "include",
    deps = [
        ":etw_provider_exporter",
        "//api",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "etw_tracer_test",
    srcs = ["test/etw_tracer_test.cc"],
    deps = [
        ":etw_tracer_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
