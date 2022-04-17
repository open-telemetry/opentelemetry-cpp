# Copyright 2020, OpenTelemetry Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

package(default_visibility = ["//visibility:public"])

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_library(
    name = "otlp_recordable",
    srcs = [
        "src/otlp_log_recordable.cc",
        "src/otlp_recordable.cc",
        "src/otlp_recordable_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_log_recordable.h",
        "include/opentelemetry/exporters/otlp/otlp_recordable.h",
        "include/opentelemetry/exporters/otlp/otlp_recordable_utils.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = ["otlp"],
    deps = [
        "//sdk/src/logs",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:logs_service_proto_cc",
        "@com_github_opentelemetry_proto//:trace_service_proto_cc",
    ],
)

cc_library(
    name = "otlp_grpc_exporter",
    srcs = [
        "src/otlp_grpc_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc",
    ],
    deps = [
        ":otlp_recordable",
        "//ext:headers",
        "//sdk/src/trace",

        # For gRPC
        "@com_github_opentelemetry_proto//:trace_service_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_library(
    name = "otlp_http_client",
    srcs = [
        "src/otlp_http_client.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_client.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    copts = [
        "-DCURL_STATICLIB",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http",
        "otlp_http_log",
    ],
    deps = [
        "//api",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk:headers",
        "@com_github_opentelemetry_proto//:common_proto_cc",
        "@github_nlohmann_json//:json",
    ],
)

cc_library(
    name = "otlp_http_exporter",
    srcs = [
        "src/otlp_http_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_exporter.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http",
    ],
    deps = [
        ":otlp_http_client",
        ":otlp_recordable",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_service_proto_cc",
    ],
)

cc_library(
    name = "otlp_http_log_exporter",
    srcs = [
        "src/otlp_http_log_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_log_exporter.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http_log",
    ],
    deps = [
        ":otlp_http_client",
        ":otlp_recordable",
        "//sdk/src/logs",
        "@com_github_opentelemetry_proto//:logs_service_proto_cc",
    ],
)

cc_library(
    name = "otlp_grpc_log_exporter",
    srcs = [
        "src/otlp_grpc_log_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_log_exporter.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc_log",
    ],
    deps = [
        ":otlp_recordable",
        "//ext:headers",
        "//sdk/src/logs",
        "@com_github_opentelemetry_proto//:logs_service_proto_cc",
        # For gRPC
        "@com_github_opentelemetry_proto//:logs_service_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_test(
    name = "otlp_recordable_test",
    srcs = ["test/otlp_recordable_test.cc"],
    tags = [
        "otlp",
        "test",
    ],
    deps = [
        ":otlp_recordable",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_log_recordable_test",
    srcs = ["test/otlp_log_recordable_test.cc"],
    tags = [
        "otlp",
        "test",
    ],
    deps = [
        ":otlp_recordable",
        "@com_github_opentelemetry_proto//:logs_service_proto_cc",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_grpc_exporter_test",
    srcs = ["test/otlp_grpc_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc",
        "test",
    ],
    deps = [
        ":otlp_grpc_exporter",
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_http_exporter_test",
    srcs = ["test/otlp_http_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_http",
        "test",
    ],
    deps = [
        ":otlp_http_exporter",
        "//api",
        "//ext/src/http/client/nosend:http_client_nosend",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_http_log_exporter_test",
    srcs = ["test/otlp_http_log_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_http_log",
        "test",
    ],
    deps = [
        ":otlp_http_log_exporter",
        "//api",
        "//ext/src/http/client/nosend:http_client_nosend",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_grpc_log_exporter_test",
    srcs = ["test/otlp_grpc_log_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc_log",
        "test",
    ],
    deps = [
        ":otlp_grpc_log_exporter",
        "//api",
        "//sdk/src/logs",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "otlp_grpc_exporter_benchmark",
    srcs = ["test/otlp_grpc_exporter_benchmark.cc"],
    tags = [
        "otlp",
        "otlp_grpc",
        "test",
    ],
    deps = [
        ":otlp_grpc_exporter",
        "//examples/common/foo_library:common_foo_library",
    ],
)
