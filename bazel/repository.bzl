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
        sha256 = "a03a7b24b3a0766dc823c9008dd32c56a1183889c04f084653266af22289ab0c",
        strip_prefix = "googletest-a6dfd3aca7f2f91f95fc7ab650c95a48420d513d",
        urls = [
            "https://github.com/google/googletest/archive/a6dfd3aca7f2f91f95fc7ab650c95a48420d513d.tar.gz",
        ],
    )

    # Load abseil dependency(optional)
    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "dd7db6815204c2a62a2160e32c55e97113b0a0178b2f090d6bab5ce36111db4b",
        strip_prefix = "abseil-cpp-20210324.0",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/20210324.0.tar.gz",
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
        name = "com_github_grpc_grpc",
        sha256 = "b74ce7d26fe187970d1d8e2c06a5d3391122f7bc1fdce569aff5e435fb8fe780",
        strip_prefix = "grpc-1.43.2",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.43.2.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        http_archive,
        name = "com_github_opentelemetry_proto",
        build_file = "@io_opentelemetry_cpp//bazel:opentelemetry_proto.BUILD",
        sha256 = "f269fbcb30e17b03caa1decd231ce826e59d7651c0f71c3b28eb5140b4bb5412",
        strip_prefix = "opentelemetry-proto-0.17.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v0.17.0.tar.gz",
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
