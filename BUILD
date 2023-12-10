# NOTE: This below code is Windows specific

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@bazel_skylib//rules:run_binary.bzl", "run_binary")
load("@rules_pkg//pkg:mappings.bzl", "pkg_filegroup", "pkg_files", pkg_strip_prefix = "strip_prefix")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
load("@aspect_bazel_lib//lib:write_source_files.bzl", "write_source_file")
load("dll_deps.bzl", "force_compilation_mode")

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

# TODO: Version needs to be updated better here.
otel_sdk_prefix = "otel_sdk/" + "1.13.0" + "/"

# Build configuration settings mimicking MSVC: debug=dbg, release=opt, reldeb=fastbuild
config_setting(
    name = "debug",
    values = {"compilation_mode": "dbg"},
)

config_setting(
    name = "release",
    values = {"compilation_mode": "opt"},
)

config_setting(
    name = "reldeb",  # Uses MSVCRT like release, not MSVCRTD like dbg
    values = {"compilation_mode": "fastbuild"},
)

# List of all extensions and exporters that are going to be linked in the otel_sdk.dll
# The transitive form of this list is used to derive DLL_DEPS in dll_deps_generated.bzl
# Which is then used to exclude (through dll_deps([])) the libs already linked in here.
# There are some gotchas, as ideally only libs that have dllexport symbols should be in there
# But it's impossible to know this in advance.
cc_library(
    name = "otel_sdk_deps",
    visibility = ["//visibility:private"],
    deps = [
        "//exporters/elasticsearch:es_log_record_exporter",
        "//exporters/etw:etw_exporter",
        "//exporters/memory:in_memory_span_exporter",  # traces
        "//exporters/ostream:ostream_log_record_exporter",
        "//exporters/ostream:ostream_metric_exporter",
        "//exporters/ostream:ostream_span_exporter",  # traces
        "//exporters/otlp:otlp_grpc_exporter",  # traces
        "//exporters/otlp:otlp_grpc_log_record_exporter",
        "//exporters/otlp:otlp_grpc_metric_exporter",
        "//exporters/otlp:otlp_http_exporter",  # traces
        "//exporters/otlp:otlp_http_log_record_exporter",
        "//exporters/otlp:otlp_http_metric_exporter",
        "//exporters/prometheus:prometheus_exporter",  # metrics
        "//exporters/zipkin:zipkin_exporter",  # traces
    ],
)

# Expands to all transitive project dependencies, excluding external projects (repos)
genquery(
    name = "otel_sdk_all_project_deps",
    # The crude '^//' ignores external repositories (e.g. @curl//, etc.) for which it's assumed we don't export dll symbols
    # In addition we exclude some internal libraries, that may have to be relinked by tests (like //sdk/src/common:random and //sdk/src/common/platform:fork)
    expression = "kind('cc_library',filter('^//',deps(//:otel_sdk_deps) except //:otel_sdk_deps except //sdk/src/common:random except //sdk/src/common/platform:fork))",
    scope = ["//:otel_sdk_deps"],
)

[cc_library(
    name = otel_sdk_binary + "_restrict_compilation_mode",
    target_compatible_with = select({
        # Makes the build target compatible only with specific compilation mode, for example:
        otel_sdk_config_name: [],  # otel_sdk_r needs "-c opt", and otel_sdk_d "-c dbg" (otel_sdk_rd the default "-c fastbuild")
        "//conditions:default": ["@platforms//:incompatible"],  # This would error out if say "bazel build -c dbg otel_sdk_r" is used.
    }),
    visibility = ["//visibility:private"],
) for (otel_sdk_binary, otel_sdk_config_name) in [
    ("otel_sdk_r", ":release"),
    ("otel_sdk_d", ":debug"),
    ("otel_sdk_rd", ":reldeb"),
]]

# Conveniently place all Open Telemetry C++ dependencies required to build otel_sdk
[cc_binary(
    name = otel_sdk_binary,

    # Almost all headers are included here, such that the compiler can notice the __declspec(dllexport) members.
    srcs = [
        "all_api_includes.cc",
        "all_sdk_includes.cc",
    ],

    # Force generation of .pdb file for for opt builds
    features = [
        "generate_pdb_file",
        # Below was an attempt to use the export all symbols feature, but it failed with:
        # LINK : fatal error LNK1189: library limit of 65535 objects exceeded
        # "windows_export_all_symbols"
    ],

    # https://learn.microsoft.com/en-us/cpp/build/reference/wholearchive-include-all-library-object-files?view=msvc-170
    linkopts = ["/WHOLEARCHIVE"],

    # Make a .dll
    linkshared = True,
    target_compatible_with = select({
        # To compile you need `--//:with_dll=true` on the command line
        "with_dll_enabled": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:private"],
    deps = [
        otel_sdk_binary + "_restrict_compilation_mode",
        "otel_sdk_deps",
    ],
) for (otel_sdk_binary, otel_sdk_config_name) in [
    ("otel_sdk_r", ":release"),
    ("otel_sdk_d", ":debug"),
    ("otel_sdk_rd", ":reldeb"),
]]

# Convenient alias that selects the appropriate otel_sdk from above
alias(
    name = "otel_sdk",
    actual = select({
        ":release": "otel_sdk_r",
        ":reldeb": "otel_sdk_rd",
        ":debug": "otel_sdk_d",
    }),
)

# Expose the import .lib file
[filegroup(
    name = otel_sdk_binary + "_lib_file",
    srcs = [otel_sdk_binary],
    output_group = "interface_library",
    visibility = ["//visibility:private"],
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

# Expose the debug .pdb file
[filegroup(
    name = otel_sdk_binary + "_pdb_file",
    srcs = [otel_sdk_binary],
    output_group = "pdb_file",
    visibility = ["//visibility:private"],
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

# Import the otel_sdk.dll, and the two exposed otel_sdk.lib and otel_sdk.pdb files as one target
[cc_import(
    name = otel_sdk_binary + "_import",
    data = [otel_sdk_binary + "_pdb_file"],
    interface_library = otel_sdk_binary + "_lib_file",
    shared_library = otel_sdk_binary,
    visibility = ["//visibility:private"],
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

# Present the import library above as cc_library so we can add headers to it, force defines, and make it public.
[cc_library(
    name = otel_sdk_binary + "_dll",
    # These have to be propagate to users of the dll library
    defines = [
        "OPENTELEMETRY_DLL=1",
        "OPENTELEMETRY_ABI_VERSION_NO=2",
        "OPENTELEMETRY_STL_VERSION=2017",
    ], #1=dllimport, -1=dllexport
    implementation_deps = [
        otel_sdk_binary + "_import",  # The otel_sdk.dll, .lib and .pdb files
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//api:headers",
        "//exporters/elasticsearch:headers",
        "//exporters/etw:headers",
        "//exporters/memory:headers",
        "//exporters/ostream:headers",
        "//exporters/otlp:headers",
        "//exporters/prometheus:headers",
        "//exporters/zipkin:headers",
        "//ext:headers",
        "//sdk:headers",
    ],
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

alias(
    name = "dll",
    actual = select({
        ":release": "otel_sdk_r_dll",
        ":reldeb": "otel_sdk_rd_dll",
        ":debug": "otel_sdk_d_dll",
    }),
    visibility = ["//visibility:public"],
)

cc_test(
    name = "dll_test",
    srcs = ["dll_test.cc"],
    deps = ["dll"],
)

pkg_files(
    name = "otel_sdk_header_files",
    srcs = [
        "//api:header_files",
        "//exporters/elasticsearch:header_files",
        "//exporters/etw:header_files",
        "//exporters/memory:header_files",
        "//exporters/ostream:header_files",
        "//exporters/otlp:header_files",
        "//exporters/prometheus:header_files",
        "//exporters/zipkin:header_files",
        "//ext:header_files",
        "//sdk:header_files",
    ],
    prefix = otel_sdk_prefix,  # + "include",
    strip_prefix = pkg_strip_prefix.from_pkg(),
)

# A "proxy" target, forcing a configuration transition to different compilation_mode.
# This allows release, debug and reldeb (e.g. opt,dbg,fastbuild) to be built and referenced together.
#
# For more info, check https://bazel.build/rules/lib/transition and otel_sdk.bzl
[force_compilation_mode(
    name = otel_sdk_binary + "_lib_file" + "_force",
    compilation_mode = compilation_mode,
    data = [otel_sdk_binary + "_lib_file"],
) for (otel_sdk_binary, compilation_mode) in [
    ("otel_sdk_r", "opt"),
    ("otel_sdk_d", "dbg"),
    ("otel_sdk_rd", "fastbuild"),
]]

# Same as above, but for the binaries
[force_compilation_mode(
    name = otel_sdk_binary + "_bin_file" + "_force",
    compilation_mode = compilation_mode,
    data = [
        otel_sdk_binary,
        otel_sdk_binary + "_pdb_file",
    ],
) for (otel_sdk_binary, compilation_mode) in [
    ("otel_sdk_r", "opt"),
    ("otel_sdk_d", "dbg"),
    ("otel_sdk_rd", "fastbuild"),
]]

# Same as above, but for the binaries
[force_compilation_mode(
    name = otel_sdk_binary + "_src_bundle" + "_force",
    compilation_mode = compilation_mode,
    data = [
        otel_sdk_binary + "_make_src_bundle",
    ],
) for (otel_sdk_binary, compilation_mode) in [
    ("otel_sdk_r", "opt"),
    ("otel_sdk_d", "dbg"),
    ("otel_sdk_rd", "fastbuild"),
]]

# Collect all sources in a .src.zip bundle using sentry-cli - https://docs.sentry.io/product/cli/dif/
[run_binary(
    name = otel_sdk_binary + "_make_src_bundle",
    srcs = [
        otel_sdk_binary + "_pdb_file",
        ".sentryclirc",
    ],
    outs = [otel_sdk_binary + ".src.zip"],
    args = [
        "debug-files",  # Called `difutil` in the online docs, but the tool lists it as `debug-files`
        "bundle-sources",
        "$(location " + otel_sdk_binary + "_pdb_file" + ")",
    ],
    tool = "@sentry_cli_windows_amd64//file:sentry-cli.exe",
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

[pkg_files(
    name = otel_sdk_binary + "_src_bundle",
    srcs = [otel_sdk_binary + "_src_bundle_force"],
    prefix = otel_sdk_prefix,
    strip_prefix = pkg_strip_prefix.from_pkg(),
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

# Package `lib` files under `otel_sdk/<version>/(debug/|reldeb/|/)lib`
[pkg_files(
    name = otel_sdk_binary + "_lib_files",
    srcs = [otel_sdk_binary + "_lib_file_force"],
    prefix = otel_sdk_prefix + prefix + "lib",
    strip_prefix = pkg_strip_prefix.from_pkg(),
) for (otel_sdk_binary, prefix) in [
    ("otel_sdk_r", ""),
    ("otel_sdk_d", "debug/"),
    ("otel_sdk_rd", "reldeb/"),
]]

# Package `bin` files under `otel_sdk/<version>/(debug/|reldeb/|/)bin`
[pkg_files(
    name = otel_sdk_binary + "_bin_files",
    srcs = [otel_sdk_binary + "_bin_file_force"],
    prefix = otel_sdk_prefix + prefix + "bin",
    strip_prefix = pkg_strip_prefix.from_pkg(),
) for (otel_sdk_binary, prefix) in [
    ("otel_sdk_r", ""),
    ("otel_sdk_d", "debug/"),
    ("otel_sdk_rd", "reldeb/"),
]]

# Group all files into one group
pkg_filegroup(
    name = "otel_sdk_files",
    srcs = [
        "otel_sdk_d_bin_files",
        "otel_sdk_d_lib_files",
        "otel_sdk_d_src_bundle",
        "otel_sdk_header_files",
        "otel_sdk_r_bin_files",
        "otel_sdk_r_lib_files",
        "otel_sdk_r_src_bundle",
        "otel_sdk_rd_bin_files",
        "otel_sdk_rd_lib_files",
        "otel_sdk_rd_src_bundle",
    ],
)

pkg_zip(
    name = "otel_sdk_zip",
    srcs = ["otel_sdk_files"],
    out = "otel_sdk.zip",
)

cc_binary(
    name = "dll_deps_update_binary",
    srcs = ["dll_deps_update.cc"],
    data = ["otel_sdk_all_project_deps"],
    local_defines = ['DEPS_FILE=\\"$(rlocationpath otel_sdk_all_project_deps)\\"'],
    deps = ["@bazel_tools//tools/cpp/runfiles"],
)

run_binary(
    name = "dll_deps_update_run",
    srcs = [":otel_sdk_all_project_deps"],
    outs = ["dll_deps_generated_internally.bzl"],
    args = ["$(location dll_deps_generated_internally.bzl)"],
    tool = "dll_deps_update_binary",
)

# To update the dll_deps_generated.bzl files, do this:
#    bazel run dll_deps_update
write_source_file(
    name = "dll_deps_update",
    in_file = "dll_deps_generated_internally.bzl",
    out_file = "dll_deps_generated.bzl",
)
