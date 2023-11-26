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
        sha256 = "6bc180a57d23d4d9515519f92b0c83d61b05b5bab188961f36ac7b06b0d9e9ce",
        strip_prefix = "benchmark-1.8.3",
        urls = [
            "https://github.com/google/benchmark/archive/v1.8.3.tar.gz",
        ],
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7",
        strip_prefix = "googletest-1.14.0",
        urls = [
            "https://github.com/google/googletest/archive/v1.14.0.tar.gz",
        ],
    )

    # Load abseil dependency(optional)
    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "987ce98f02eefbaf930d6e38ab16aa05737234d7afbab2d5c4ea7adbe50c28ed",
        strip_prefix = "abseil-cpp-20230802.1",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/refs/tags/20230802.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        #sha256 = "5086b72b285bfeb6feb71a39dc5e8e452a64bf477eb5054d767c164b366b0c52",
        strip_prefix = "grpc-1.52.2",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.52.2.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        http_archive,
        name = "com_github_opentelemetry_proto",
        build_file = "@io_opentelemetry_cpp//bazel:opentelemetry_proto.BUILD",
        sha256 = "a13a1a7b76a1f22a0ca2e6c293e176ffef031413ab8ba653a82a1dbc286a3a33",
        strip_prefix = "opentelemetry-proto-1.0.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v1.0.0.tar.gz",
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
        sha256 = "8150406605389ececb6da07cbcb509d5637a3ab9a24bc69b1101531367d89d74",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
            "https://github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
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
        sha256 = "476303bd0f1b04cc311fc258f1708a5f6ef82d3091e53fd1977fa20383425a6a",
        strip_prefix = "rules_foreign_cc-0.10.1",
        url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.10.1/rules_foreign_cc-0.10.1.tar.gz",
    )

    # bazel skylib
    maybe(
        http_archive,
        name = "bazel_skylib",
        sha256 = "cd55a062e763b9349921f0f5db8c3933288dc8ba4f76dd9416aac68acee3cb94",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
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
