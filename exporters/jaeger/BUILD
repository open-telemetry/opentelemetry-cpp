package(default_visibility = ["//visibility:public"])

load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake", "configure_make", "configure_make_variant")

CONFIGURE_OPTIONS = [
    "shared",
    "no-weak-ssl-ciphers",
    "no-idea",
    "no-comp",
]

LIB_NAME = "openssl"

MAKE_TARGETS = [
    "build_libs",
    "install_dev",
]

config_setting(
    name = "msvc_compiler",
    flag_values = {
        "@bazel_tools//tools/cpp:compiler": "msvc-cl",
    },
    visibility = ["//visibility:public"],
)

alias(
    name = "openssl",
    actual = select({
        ":msvc_compiler": "openssl_msvc",
        "//conditions:default": "openssl_default",
    }),
    visibility = ["//visibility:public"],
)

configure_make_variant(
    name = "openssl_msvc",
    build_data = [
        ":nasm",
        ":perl",
    ],
    configure_command = "Configure",
    configure_in_place = True,
    configure_options = CONFIGURE_OPTIONS + [
        "VC-WIN64A",
        # Unset Microsoft Assembler (MASM) flags set by built-in MSVC toolchain,
        # as NASM is used to build OpenSSL rather than MASM
        "ASFLAGS=\" \"",
    ],
    configure_prefix = "$PERL",
    env = {
        # The Zi flag must be set otherwise OpenSSL fails to build due to missing .pdb files
        "CFLAGS": "-Zi",
        "PATH": "$$(dirname $(execpath @nasm//:nasm)):$$PATH",
        "PERL": "$(execpath @perl//:perl)",
    },
    lib_name = LIB_NAME,
    lib_source = ":all_srcs",
    out_static_libs = [
        "libssl.lib",
        "libcrypto.lib",
    ],
    target_compatible_with = [
        "@bazel_tools//platforms:windows",
    ],
    targets = MAKE_TARGETS,
    toolchain = "@rules_foreign_cc//toolchains:preinstalled_nmake_toolchain",
)

configure_make(
    name = "openssl_default",
    configure_command = "config",
    configure_in_place = True,
    configure_options = select({
        "@bazel_tools//platforms:osx": [
            "no-afalgeng",
            "ARFLAGS=r",
        ] + CONFIGURE_OPTIONS,
        "@bazel_tools//platforms:linux": [
        ] + CONFIGURE_OPTIONS,
    }),
    copts = [
        "-Ilibs/exporters/jaeger/openssl/include",
    ],
    env = select({
        "@bazel_tools//platforms:osx": {
            "AR": "",
        },
        "@bazel_tools//platforms:linux": {},
    }),
    lib_source = "@com_github_openssl//:all_srcs",
    out_shared_libs = select({
        "@bazel_tools//platforms:osx": [
            "libssl.dylib",
            "libcrypto.dylib",
        ],
        "@bazel_tools//platforms:linux": [
            "libssl.so",
            "libcrypto.so",
        ],
    }),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "openssl_files",
    srcs = [
        ":openssl",
    ],
    output_group = "gen_dir",
    visibility = [
        "//visibility:public",
    ],
)

genrule(
    name = "openssl_includes",
    srcs = [
        ":openssl_files",
    ],
    outs = ["include"],
    cmd = "cp -r $(location openssl_files)/include/ $(location include)",
)

cc_library(
    name = "openssl_header_only",
    hdrs = [
        ":openssl_includes",
    ],
    copts = ["-isystem include"],
)

cmake(
    name = "thrift",
    cache_entries = {
        "CMAKE_BUILD_TYPE": "Release",
        "BUILD_COMPILER": "OFF",
        "BUILD_CPP": "ON",
        "BUILD_LIBRARIES": "ON",
        "BUILD_NODEJS": "OFF",
        "BUILD_PYTHON": "OFF",
        "BUILD_JAVASCRIPT": "OFF",
        "BUILD_C_GLIB": "OFF",
        "BUILD_JAVA": "OFF",
        "BUILD_TESTING": "OFF",
        "BUILD_TUTORIALS": "OFF",
        "WITH_HASKELL": "OFF",
    },
    copts = [
        "-Ilibs/exporters/jaeger/openssl/include",
    ],
    lib_source = "@com_github_thrift//:all_srcs",
    out_static_libs = [
        "libthrift.a",
        "libthriftz.a",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":openssl",
        ":openssl_header_only",
        "@boost//:locale",
        "@boost//:numeric_conversion",
    ],
)

cc_library(
    name = "jaeger_thrift_gencpp",
    srcs = [
        "thrift-gen/Agent.cpp",
        "thrift-gen/Collector.cpp",
        "thrift-gen/ZipkinCollector.cpp",
        "thrift-gen/jaeger_types.cpp",
        "thrift-gen/zipkincore_constants.cpp",
        "thrift-gen/zipkincore_types.cpp",
    ],
    hdrs = [
        "thrift-gen/Agent.h",
        "thrift-gen/Collector.h",
        "thrift-gen/ZipkinCollector.h",
        "thrift-gen/agent_types.h",
        "thrift-gen/jaeger_types.h",
        "thrift-gen/zipkincore_constants.h",
        "thrift-gen/zipkincore_types.h",
    ],
    copts = [
        "-fexceptions",
    ],
    strip_include_prefix = "thrift-gen",
    tags = ["jaeger"],
    deps = [
        ":thrift",
        "//ext/src/http/client/curl:http_client_curl",
    ],
)

cc_library(
    name = "jaeger_exporter",
    srcs = [
    ],
    hdrs = [
        "src/THttpTransport.h",
        "src/TUDPTransport.h",
        "src/http_transport.h",
        "src/sender.h",
        "src/thrift_sender.h",
        "src/transport.h",
        "src/udp_transport.h",
    ],
    copts = ["-fexceptions"],
    strip_include_prefix = "src",
    tags = ["jaeger"],
    deps = [
        ":jaeger_thrift_gencpp",
    ],
)

cc_library(
    name = "opentelemetry_exporter_jaeger_trace",
    srcs = [
        "src/THttpTransport.cc",
        "src/TUDPTransport.cc",
        "src/http_transport.cc",
        "src/jaeger_exporter.cc",
        "src/recordable.cc",
        "src/thrift_sender.cc",
        "src/udp_transport.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/jaeger/jaeger_exporter.h",
        "include/opentelemetry/exporters/jaeger/recordable.h",
    ],
    copts = ["-fexceptions"],
    strip_include_prefix = "include",
    tags = ["jaeger"],
    deps = [
        ":jaeger_exporter",
    ],
)
