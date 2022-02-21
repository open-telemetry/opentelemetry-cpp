load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def openssl_repositories():
    maybe(
        http_archive,
        name = "nasm",
        build_file_content = """
load("@bazel_skylib//rules:select_file.bzl", "select_file")
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)
select_file(
    name = "nasm",
    srcs = ":all_srcs",
    subpath = "nasm.exe",
)""",
        sha256 = "f5c93c146f52b4f1664fa3ce6579f961a910e869ab0dae431bd871bdd2584ef2",
        strip_prefix = "nasm-2.15.05",
        urls = [
            "https://mirror.bazel.build/www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/nasm-2.15.05-win64.zip",
            "https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/nasm-2.15.05-win64.zip",
        ],
    )
    maybe(
        http_archive,
        name = "rules_perl",
        sha256 = "55fbe071971772758ad669615fc9aac9b126db6ae45909f0f36de499f6201dd3",
        strip_prefix = "rules_perl-2f4f36f454375e678e81e5ca465d4d497c5c02da",
        urls = [
            "https://github.com/bazelbuild/rules_perl/archive/2f4f36f454375e678e81e5ca465d4d497c5c02da.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "perl",
        build_file_content = """
load("@bazel_skylib//rules:select_file.bzl", "select_file")
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)
select_file(
    name = "perl",
    srcs = ":all_srcs",
    subpath = "perl/bin/perl.exe",
)
        """,
        sha256 = "aeb973da474f14210d3e1a1f942dcf779e2ae7e71e4c535e6c53ebabe632cc98",
        urls = [
            "https://mirror.bazel.build/strawberryperl.com/download/5.32.1.1/strawberry-perl-5.32.1.1-64bit.zip",
            "https://strawberryperl.com/download/5.32.1.1/strawberry-perl-5.32.1.1-64bit.zip",
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
        path = "vcpkg/installed/x64-windows/",
    )
