# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

workspace(name = "io_opentelemetry_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

http_archive(
    name = "aspect_bazel_lib",
    sha256 = "c4f36285ceed51f75da44ffcf8fa393794d0dc2e273a2e03be50462e347740cd",
    strip_prefix = "bazel-lib-2.0.0",
    url = "https://github.com/aspect-build/bazel-lib/releases/download/v2.0.0/bazel-lib-v2.0.0.tar.gz",
)

load("@aspect_bazel_lib//lib:repositories.bzl", "aspect_bazel_lib_dependencies", "aspect_bazel_lib_register_toolchains")

# Required bazel-lib dependencies

aspect_bazel_lib_dependencies()

# Register bazel-lib toolchains

aspect_bazel_lib_register_toolchains()

http_archive(
    name = "bazel_skylib",
    sha256 = "cd55a062e763b9349921f0f5db8c3933288dc8ba4f76dd9416aac68acee3cb94",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_pkg",
    sha256 = "8f9ee2dc10c1ae514ee599a8b42ed99fa262b757058f65ad3c384289ff70c4b8",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.9.1/rules_pkg-0.9.1.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.9.1/rules_pkg-0.9.1.tar.gz",
    ],
)

load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()

# Rules apple for bazel 7.0.0, as the project somehow pulls by default too old rules - 0.32.0

http_archive(
    name = "build_bazel_rules_apple",
    sha256 = "34c41bfb59cdaea29ac2df5a2fa79e5add609c71bb303b2ebb10985f93fa20e7",
    url = "https://github.com/bazelbuild/rules_apple/releases/download/3.1.1/rules_apple.3.1.1.tar.gz",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

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

http_file(
    name = "sentry_cli_windows_amd64",
    downloaded_file_path = "sentry-cli.exe",
    executable = True,
    sha256 = "bc51a87a60fa13a619ecaa93f9d442ae7e638a1718cb4616c6595ce7ab02ab25",
    urls = [
        "https://github.com/getsentry/sentry-cli/releases/download/2.23.0/sentry-cli-Windows-x86_64.exe",
    ],
)

# Use clang-cl for compilation. The alternative form is now in the .bazelrc, this is kept here just for reference
# The toolchain config is in the BUILD file
#
# register_execution_platforms(
#     ":x64_windows-clang-cl",
# )
# register_toolchains(
#     "@local_config_cc//:cc-toolchain-x64_windows-clang-cl",
# )


