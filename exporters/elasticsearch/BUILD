load("//:dll_deps.bzl", "dll_deps")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "es_log_record_exporter",
    srcs = [
        "src/es_log_record_exporter.cc",
        "src/es_log_recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/elasticsearch/es_log_record_exporter.h",
        "include/opentelemetry/exporters/elasticsearch/es_log_recordable.h",
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
    tags = ["es"],
    deps = [
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/logs",
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "es_log_record_exporter_test",
    srcs = ["test/es_log_record_exporter_test.cc"],
    tags = [
        "es",
        "test",
    ],
    deps = dll_deps([
        ":es_log_record_exporter",
    ]) + [
        "@github_nlohmann_json//:json",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
