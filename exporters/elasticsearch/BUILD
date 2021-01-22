package(default_visibility = ["//visibility:public"])

cc_library(
    name = "es_log_exporter",
    srcs = [
        "src/es_log_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/es_log_exporter.h",
        "include/opentelemetry/exporters/elasticsearch/es_log_recordable.h",
    ],
    copts = [
        "-DCURL_STATICLIB",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    deps = [
        "//ext:headers",
        "//ext/http/client/curl/http_client_curl",
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
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
