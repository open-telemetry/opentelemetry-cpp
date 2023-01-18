load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "with_dll",
    build_setting_default = False,
)

config_setting(
    name = "with_dll_enabled",
    flag_values = {
        "with_dll": "true",
        "//api:with_abseil": "false",
    },
)

cc_library(
    name = "otel_sdk_deps",
    target_compatible_with = select({
        # To compile you need `--//:with_dll=true` on the command line
        "with_dll_enabled": [],
        "//conditions:default": ["@platforms//:incompatible"]
    }),
    deps = [
        "//api",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_metric_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//exporters/otlp:otlp_grpc_exporter",
        "//exporters/otlp:otlp_grpc_metric_exporter",
        "//exporters/otlp:otlp_http_exporter",
        "//exporters/otlp:otlp_http_metric_exporter",
#        "//exporters/prometheus:prometheus_exporter",
        "//exporters/zipkin:zipkin_exporter",
        "//ext/src/http/client/curl:http_client_curl",
        "//ext/src/zpages",
    ],
    alwayslink = 1,
    visibility = ["//visibility:private"],
)

# Conveniently place all Open Telemetry C++ dependencies required to build otel_sdk
cc_binary(
    name = "otel_sdk",
    deps = [
        "otel_sdk_deps",
    ],

    # Force generation of .pdb file for for opt builds.
    features = ["generate_pdb_file"],

    # Make a .dll
    linkshared = True,

    linkopts = ["/WHOLEARCHIVE"],

    # Almost all headers are included here, such that the compiler can notice the __declspec(dllexport) members.
    srcs = ["all_api_includes.cc", "all_sdk_includes.cc"],
    visibility = ["//visibility:private"],
)

# Expose the import .lib file
filegroup(
    name = "otel_sdk_lib_file",
    srcs = ["otel_sdk"],
    output_group = "interface_library",
    visibility = ["//visibility:private"],
)

# Expose the debug .pdb file
filegroup(
    name = "otel_sdk_pdb_file",
    srcs = ["otel_sdk"],
    output_group = "pdb_file",
    visibility = ["//visibility:private"],
)

# Import the otel_sdk.dll, and the two exposed otel_sdk.lib and otel_sdk.pdb files as one target
cc_import(
    name = "otel_sdk_import",
    interface_library = "otel_sdk_lib_file",
    shared_library = "otel_sdk",
    data = ["otel_sdk_pdb_file"],
    visibility = ["//visibility:private"],
)

# Present the import library above as cc_library so we can add headers to it, force defines, and make it public.
cc_library(
    name = "dll",
    # These have to be propagate to users of the dll library
    defines = ["OPENTELEMETRY_DLL=1", "HAVE_CPP_STDLIB=1"], 
    implementation_deps = [
        "otel_sdk_import", # The otel_sdk.dll, .lib and .pdb files
    ],
    deps = [
        "//api:headers",
        "//sdk:headers",
        "//ext:headers",
        "//exporters/memory:headers",
        "//exporters/ostream:headers",
        "//exporters/zipkin:headers",
#        "//exporters/prometheus:headers",
        "//exporters/otlp:headers",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "dll_test",
    srcs = ["dll_test.cc"],
    deps = ["dll"],
)
