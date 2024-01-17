# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

_ALL_CONTENT = """
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""

#
# MAINTAINER
#
# This file is used for the Bazel build.
#
# When changing (add, upgrade, remove) dependencies
# please update:
# - the CMake build, see file
#   <root>/third_party_release
# - git submodule, see command
#   git submodule status
#

def opentelemetry_cpp_deps():
    """Loads dependencies need to compile the opentelemetry-cpp library."""

    # Google Benchmark library.
    # Only needed for benchmarks, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "6430e4092653380d9dc4ccb45a1e2dc9259d581f4866dc0759713126056bc1d7",
        strip_prefix = "benchmark-1.7.1",
        urls = [
            "https://github.com/google/benchmark/archive/v1.7.1.tar.gz",
        ],
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "ad7fdba11ea011c1d925b3289cf4af2c66a352e18d4c7264392fead75e919363",
        strip_prefix = "googletest-1.13.0",
        urls = [
            "https://github.com/google/googletest/archive/v1.13.0.tar.gz",
        ],
    )

    # Load abseil dependency(optional)
    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "91ac87d30cc6d79f9ab974c51874a704de9c2647c40f6932597329a282217ba8",
        strip_prefix = "abseil-cpp-20220623.1",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/refs/tags/20220623.1.tar.gz",
        ],
    )

    # Load gRPC dependency
    maybe(
        http_archive,
        name = "com_github_grpc_grpc_legacy",
        sha256 = "024118069912358e60722a2b7e507e9c3b51eeaeee06e2dd9d95d9c16f6639ec",
        strip_prefix = "grpc-1.39.1",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.39.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc_latest11",
        sha256 = "e266aa0d9d9cddb876484a370b94f468248594a96ca0b6f87c21f969db2b8c5b",
        strip_prefix = "grpc-1.46.4",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.46.4.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        sha256 = "cdeb805385fba23242bf87073e68d590c446751e09089f26e5e0b3f655b0f089",
        strip_prefix = "grpc-1.49.2",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.49.2.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        http_archive,
        name = "com_github_opentelemetry_proto",
        build_file = "@io_opentelemetry_cpp//bazel:opentelemetry_proto.BUILD",
        sha256 = "df491a05f3fcbf86cc5ba5c9de81f6a624d74d4773d7009d573e37d6e2b6af64",
        strip_prefix = "opentelemetry-proto-1.1.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v1.1.0.tar.gz",
        ],
    )

    # JSON library
    maybe(
        http_archive,
        name = "github_nlohmann_json",
        build_file = "@io_opentelemetry_cpp//bazel:nlohmann_json.BUILD",
        sha256 = "e5c7a9f49a16814be27e4ed0ee900ecd0092bfb7dbfca65b5a421b774dccaaed",
        urls = [
            "https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip",
        ],
    )

    # C++ Prometheus Client library.
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        sha256 = "397544fe91e183029120b4eebcfab24ed9ec833d15850aae78fd5db19062d13a",
        strip_prefix = "prometheus-cpp-1.1.0",
        urls = [
            "https://github.com/jupp0r/prometheus-cpp/archive/refs/tags/v1.1.0.tar.gz",
        ],
    )

    # bazel platforms
    maybe(
        http_archive,
        name = "platforms",
        sha256 = "5308fc1d8865406a49427ba24a9ab53087f17f5266a7aabbfc28823f3916e1ca",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
            "https://github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
        ],
    )

    # libcurl (optional)
    maybe(
        http_archive,
        name = "curl",
        build_file = "@io_opentelemetry_cpp//bazel:curl.BUILD",
        sha256 = "816e41809c043ff285e8c0f06a75a1fa250211bbfb2dc0a037eeef39f1a9e427",
        strip_prefix = "curl-8.4.0",
        urls = [
            "https://curl.haxx.se/download/curl-8.4.0.tar.gz",
            "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.tar.gz",
        ],
    )

    # rules foreign cc
    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "69023642d5781c68911beda769f91fcbc8ca48711db935a75da7f6536b65047f",
        strip_prefix = "rules_foreign_cc-0.6.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.6.0.tar.gz",
    )

    # bazel skylib
    maybe(
        http_archive,
        name = "bazel_skylib",
        sha256 = "c6966ec828da198c5d9adbaa94c05e3a1c7f21bd012a0b29ba8ddbccb2c93b0d",
        urls = [
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
        ],
    )

    # Opentracing
    maybe(
        http_archive,
        name = "com_github_opentracing",
        sha256 = "5b170042da4d1c4c231df6594da120875429d5231e9baa5179822ee8d1054ac3",
        strip_prefix = "opentracing-cpp-1.6.0",
        urls = [
            "https://github.com/opentracing/opentracing-cpp/archive/refs/tags/v1.6.0.tar.gz",
        ],
    )
