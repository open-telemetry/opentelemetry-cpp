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
        sha256 = "1f71c72ce08d2c1310011ea6436b31e39ccab8c2db94186d26657d41747c85d6",
        strip_prefix = "benchmark-1.6.0",
        urls = [
            "https://github.com/google/benchmark/archive/v1.6.0.tar.gz",
        ],
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2",
        strip_prefix = "googletest-release-1.12.1",
        urls = [
            "https://github.com/google/googletest/archive/release-1.12.1.tar.gz",
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
        sha256 = "464bc2b348e674a1a03142e403cbccb01be8655b6de0f8bfe733ea31fcd421be",
        strip_prefix = "opentelemetry-proto-0.19.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v0.19.0.tar.gz",
        ],
    )

    # JSON library
    maybe(
        http_archive,
        name = "github_nlohmann_json",
        build_file = "@io_opentelemetry_cpp//bazel:nlohmann_json.BUILD",
        sha256 = "b94997df68856753b72f0d7a3703b7d484d4745c567f3584ef97c96c25a5798e",
        urls = [
            "https://github.com/nlohmann/json/releases/download/v3.10.5/include.zip",
        ],
    )

    # C++ Prometheus Client library.
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        sha256 = "07018db604ea3e61f5078583e87c80932ea10c300d979061490ee1b7dc8e3a41",
        strip_prefix = "prometheus-cpp-1.0.0",
        urls = [
            "https://github.com/jupp0r/prometheus-cpp/archive/refs/tags/v1.0.0.tar.gz",
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
        sha256 = "ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91",
        strip_prefix = "curl-7.73.0",
        urls = ["https://curl.haxx.se/download/curl-7.73.0.tar.gz"],
    )

    # libthrift (optional)
    maybe(
        http_archive,
        name = "com_github_thrift",
        build_file_content = _ALL_CONTENT,
        sha256 = "5ae1c4d16452a22eaf9d802ba7489907147c2b316ff38c9758918552fae5132c",
        strip_prefix = "thrift-0.14.1",
        urls = [
            "https://github.com/apache/thrift/archive/refs/tags/v0.14.1.tar.gz",
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

    # boost headers from vcpkg
    maybe(
        native.new_local_repository,
        name = "boost_all_hdrs",
        build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
  name = "boost_all_hdrs",
  hdrs = glob(
      ["include/**/*.h*"],
  ),
  strip_include_prefix = "include",
  copts = [
      "-isystem include",
      "-fexceptions",
    ],
    visibility = ["//visibility:public"],
)
        """,
        path = "tools/vcpkg/installed/x64-windows/",
    )
