package(default_visibility = ["//visibility:public"])

load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake", "configure_make")

configure_make(
    name = "openssl",
    configure_command = "config",
    configure_options = [
        "shared",
    ],
    lib_source = "@openssl//:all_srcs",
    out_shared_libs = select({
        "@platforms//os:windows": [
            "libssl.dll",
            "libcrypto.dll",
        ],
        "//conditions:default": [
            "libssl.so",
            "libcrypto.so",
        ],
    }),
    tags = ["jaeger"],
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
        "CMAKE_CXX_FLAGS": "-fexceptions",
    },
    copts = ["-fexceptions"],
    lib_source = "@thrift//:all_srcs",
    out_static_libs = select({
        "@platforms//os:windows": [
            "libthrift.lib",
            "libthriftnb.ĺib",
            "libthriftz.lib",
        ],
        "//conditions:default": [
            "libthrift.a",
            "libthriftnb.a",
            "libthriftz.a",
        ],
    }),
    tags = ["jaeger"],
    visibility = ["//visibility:private"],
    deps = [
        ":openssl",
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
    copts = ["-fexceptions"],
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
