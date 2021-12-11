package(default_visibility = ["//visibility:public"])

load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake", "configure_make", "configure_make_variant")

CONFIGURE_OPTIONS = [
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
    name = "windows",
    constraint_values = [
        "@bazel_tools//platforms:windows",
    ],
    visibility = ["//visibility:private"],
)

alias(
    name = "openssl",
    actual = select({
        ":windows": "openssl_msvc",
        "//conditions:default": "openssl_default",
    }),
    visibility = ["//visibility:public"],
)

configure_make_variant(
    name = "openssl_msvc",
    build_data = [
        "@nasm//:nasm",
        "@perl//:perl",
    ],
    configure_command = "Configure",
    configure_in_place = True,
    configure_options = CONFIGURE_OPTIONS + [
        "no-shared",
        "VC-WIN64A",
        # Unset Microsoft Assembler (MASM) flags set by built-in MSVC toolchain,
        # as NASM is used to build OpenSSL rather than MASM
        "ASFLAGS=\"-g\"",
        "CFLAGS=\"/W3 /wd4090 /nologo /O2 /Zi /MP\"",
        "LDFLAGS=\"/nologo /debug /Zi\"",
        "ARFLAGS=\"/nologo\"",
        "LD=\"link\"",
    ],
    configure_prefix = "$PERL",
    env = {
        # The Zi flag must be set otherwise OpenSSL fails to build due to missing .pdb files
        "CFLAGS": "-Zi",
        "PATH": "$(dirname $(execpath @nasm//:nasm)):$PATH",
        "PERL": "$(execpath @perl//:perl)",
        "CL": "/MP"
    },
    lib_name = LIB_NAME,
    lib_source = "@com_github_openssl//:all_srcs",
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
            "shared",
            "no-afalgeng",
            "ARFLAGS=r",
        ] + CONFIGURE_OPTIONS,
        "@bazel_tools//platforms:linux": [
            "shared",
        ] + CONFIGURE_OPTIONS,
        "@bazel_tools//platforms:windows": [
            "shared",
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
        "@bazel_tools//platforms:windows": {},
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
        "@bazel_tools//platforms:windows": [
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

THRIFT_CACHE_ENTRIES = {
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
}

THRIFT_CACHE_ENTRIES_WIN = THRIFT_CACHE_ENTRIES
THRIFT_CACHE_ENTRIES_WIN["CMAKE_TOOLCHAIN_FILE"] = "D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
THRIFT_DEPS_WIN = [
    ":openssl",
    ":openssl_header_only",
]

THRIFT_DEPS = THRIFT_DEPS_WIN + [
    "@boost//:locale",
    "@boost//:numeric_conversion",
]

cmake(
    name = "thrift",
    cache_entries = select({
        "@bazel_tools//platforms:osx": THRIFT_CACHE_ENTRIES,
        "@bazel_tools//platforms:linux": THRIFT_CACHE_ENTRIES,
        "@bazel_tools//platforms:windows": THRIFT_CACHE_ENTRIES_WIN,
    }),
    copts = [
        "-Ilibs/exporters/jaeger/openssl/include",
    ],
    lib_source = "@com_github_thrift//:all_srcs",
    out_static_libs = select({
        "@bazel_tools//platforms:osx": [
            "libthrift.a",
            "libthriftz.a",
        ],
        "@bazel_tools//platforms:linux": [
            "libthrift.a",
            "libthriftz.a",
        ],
        "@bazel_tools//platforms:windows": [
            "thriftmd.lib",
            "thriftnbmd.lib",
            "thriftzmd.lib"
        ],
    }),
    visibility = ["//visibility:public"],
    # generate_args = ["-G \"NMake Makefiles\""],
    generate_args = [
        "-G \"Visual Studio 15 2017\"",                                                                           
        "-A x64",
    ],
    out_lib_dir = "bin",
    deps = select({
        "@bazel_tools//platforms:osx": THRIFT_DEPS,
        "@bazel_tools//platforms:linux": THRIFT_DEPS,
        "@bazel_tools//platforms:windows": THRIFT_DEPS_WIN,
    }),
    install = True,
)

THRIFT_GEN_DEPS = [
    ":thrift",
    "//ext/src/http/client/curl:http_client_curl",
]

THRIFT_GEN_DEPS_WIN = THRIFT_GEN_DEPS + [
    "@boost//:locale",
    "@boost//:numeric_conversion",
]

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
    deps = select({
        "@bazel_tools//platforms:osx": THRIFT_GEN_DEPS,
        "@bazel_tools//platforms:linux": THRIFT_GEN_DEPS,
        "@bazel_tools//platforms:windows": THRIFT_GEN_DEPS_WIN,
    }),
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

cc_test(
    name = "jaeger_recordable_test",
    srcs = ["test/jaeger_recordable_test.cc"],
    tags = [
        "jaeger",
        "test",
    ],
    deps = [
        ":opentelemetry_exporter_jaeger_trace",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)
