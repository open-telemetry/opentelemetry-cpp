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
    maybe(
        http_archive,
        name = "civetweb",
        strip_prefix = "civetweb-1.16",
        sha256 = "f0e471c1bf4e7804a6cfb41ea9d13e7d623b2bcc7bc1e2a4dd54951a24d60285",
        urls = [
            "https://github.com/civetweb/civetweb/archive/v1.16.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:civetweb.BUILD",
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        sha256 = "17e4e1b100657b88027721220cbfb694d86c4b807e9257eaf2fb2d273b41b1b1",
        strip_prefix = "grpc-1.54.3",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.54.3.tar.gz",
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

    maybe(
        http_archive,
        name = "zlib",
        sha256 = "b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30",
        strip_prefix = "zlib-1.2.13",
        urls = [
            "https://mirror.bazel.build/zlib.net/zlib-1.2.13.tar.gz",
            "https://zlib.net/zlib-1.2.13.tar.gz",
            "https://storage.googleapis.com/bazel-mirror/zlib.net/zlib-1.2.13.tar.gz",
        ],
        build_file = "@com_github_jupp0r_prometheus_cpp//bazel:zlib.BUILD",
    )

    # C++ Prometheus Client library.
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        integrity = "sha256-Yp/fRT8CJvaTwI37Xcra582bOWiXrywPJ6zRUIg9d5w=",
        strip_prefix = "prometheus-cpp-with-submodules",
        urls = [
            "https://github.com/jupp0r/prometheus-cpp/releases/download/v1.2.0/prometheus-cpp-with-submodules.tar.gz",
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
