package(default_visibility = ["//visibility:public"])

cc_library(
    name = "es_log_exporter",
    srcs = [
        "src/es_log_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/es_log_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//ext:headers",
        "//sdk/src/logs",
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "es_log_exporter_test",
    srcs = ["test/es_log_exporter_test.cc"],
    deps = [
        ":es_log_exporter",
        "@curl",
        "@com_google_googletest//:gtest_main",
    ],
)
