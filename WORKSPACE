# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

workspace(name = "io_opentelemetry_cpp")

# This overrides jupp0r/prometheus own use of zlib, from the @net_zlib_zlib//:z target
# in the bazel/net_zlib_overide.BUILD file, there is 'z' target that is alias to @zlib//:zlib (used by grpc)
new_local_repository(
    name = "net_zlib_zlib",
    build_file = "//bazel:net_zlib_override.BUILD",
    # Feel weird about this, but it's needed.
    path = "",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "aspect_bazel_lib",
    sha256 = "ef83252dea2ed8254c27e65124b756fc9476be2b73a7799b7a2a0935937fc573",
    strip_prefix = "bazel-lib-1.24.2",
    url = "https://github.com/aspect-build/bazel-lib/releases/download/v1.24.2/bazel-lib-v1.24.2.tar.gz",
)
load("@aspect_bazel_lib//lib:repositories.bzl", "aspect_bazel_lib_dependencies", "register_jq_toolchains", "register_yq_toolchains")
aspect_bazel_lib_dependencies()
register_jq_toolchains()
register_yq_toolchains()

http_archive(
    name = "bazel_skylib",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
    ],
    sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

http_archive(
    name = "rules_pkg",
    sha256 = "eea0f59c28a9241156a47d7a8e32db9122f3d50b505fae0f33de6ce4d9b61834",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.8.0/rules_pkg-0.8.0.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.8.0/rules_pkg-0.8.0.tar.gz",
    ],
)
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()

# Load our direct dependencies.
load("//bazel:repository.bzl", "opentelemetry_cpp_deps")

opentelemetry_cpp_deps()

load("//bazel:extra_deps.bzl", "opentelemetry_extra_deps")

opentelemetry_extra_deps()

# Load gRPC dependencies after load.
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

# Load extra gRPC dependencies due to https://github.com/grpc/grpc/issues/20511
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
