# NOTE: This below code is Windows specific

load("@aspect_bazel_lib//lib:write_source_files.bzl", "write_source_file")
load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@bazel_skylib//rules:run_binary.bzl", "run_binary")
load("@bazel_skylib//rules:native_binary.bzl", "native_binary")
load("@otel_sdk//bazel:otel_cc.bzl", "otel_cc_binary", "otel_cc_import", "otel_cc_library", "otel_cc_shared_library", "otel_cc_test")
load("@rules_pkg//pkg:mappings.bzl", "pkg_filegroup", "pkg_files", pkg_strip_prefix = "strip_prefix")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")
load("dll_deps.bzl", "force_compilation_mode")

package(default_visibility = ["//visibility:public"])

bool_flag(
    name = "with_dll",
    build_setting_default = False,
)

config_setting(
    name = "with_dll_enabled",
    flag_values = {
        "with_dll": "true",
    },
)

otel_cc_library(
    name = "windows_only",
    target_compatible_with = select({
        "@platforms//os:windows": None,
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)

# TODO: Version needs to be updated better here.
otel_sdk_prefix = "otel_sdk/" + "1.17.0" + "/"

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
otel_cc_library(
    name = "otel_sdk_deps",
    target_compatible_with = select({
        # To compile you need `--//:with_dll=true` on the command line
        "with_dll_enabled": None,
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:private"],
    deps = [
        "@otel_sdk//exporters/elasticsearch:es_log_record_exporter",
        "@otel_sdk//exporters/memory:in_memory_data",
        "@otel_sdk//exporters/memory:in_memory_metric_data",
        "@otel_sdk//exporters/memory:in_memory_metric_exporter_factory",
        "@otel_sdk//exporters/memory:in_memory_span_exporter",
        "@otel_sdk//exporters/ostream:ostream_log_record_exporter",
        "@otel_sdk//exporters/ostream:ostream_metric_exporter",
        "@otel_sdk//exporters/ostream:ostream_span_exporter",
        "@otel_sdk//exporters/otlp:otlp_file_exporter",
        "@otel_sdk//exporters/otlp:otlp_file_log_record_exporter",
        "@otel_sdk//exporters/otlp:otlp_file_metric_exporter",
        "@otel_sdk//exporters/otlp:otlp_grpc_exporter",
        "@otel_sdk//exporters/otlp:otlp_grpc_log_record_exporter",
        "@otel_sdk//exporters/otlp:otlp_grpc_metric_exporter",
        "@otel_sdk//exporters/otlp:otlp_grpc_forward_proxy",
        "@otel_sdk//exporters/otlp:otlp_http_exporter",
        "@otel_sdk//exporters/otlp:otlp_http_log_record_exporter",
        "@otel_sdk//exporters/otlp:otlp_http_metric_exporter",
        "@otel_sdk//exporters/prometheus:prometheus_exporter",
        "@otel_sdk//exporters/prometheus:prometheus_push_exporter",
        "@otel_sdk//exporters/zipkin:zipkin_exporter",
    ] + select({
        "@platforms//os:windows": ["@otel_sdk//exporters/etw:etw_exporter"],
        "//conditions:default": [],
    }),
)

# Expands to all transitive project dependencies, excluding external projects (repos)
[genquery(
    name = "otel_sdk_all_deps_" + os,
    # The crude '^(@+otel_sdk[+~]?)?//' ignores external to otel_sdk repositories (e.g. @curl//, etc.) for which it's assumed we don't export dll symbols
    # In addition we exclude some internal libraries, that may have to be relinked by tests (like //sdk/src/common:random and //sdk/src/common/platform:fork)
    expression = "kind('cc_library',filter('^(@+otel_sdk[+~]?)?//',deps(@otel_sdk//:otel_sdk_deps) except set(@otel_sdk//:otel_sdk_deps @otel_sdk//sdk/src/common:random @otel_sdk//sdk/src/common/platform:fork @otel_sdk//:windows_only " + exceptions + ")))",
    strict = True,
    scope = ["@otel_sdk//:otel_sdk_deps"],
) for (os, exceptions) in [
    ("non_windows", "@otel_sdk//exporters/etw:etw_exporter"),
    ("windows", ""),
]]

[otel_cc_library(
    name = otel_sdk_binary + "_restrict_compilation_mode",
    target_compatible_with = select({
        # Makes the build target compatible only with specific compilation mode, for example:
        otel_sdk_config_name: None,  # otel_sdk_r needs "-c opt", and otel_sdk_d "-c dbg" (otel_sdk_rd the default "-c fastbuild")
        "//conditions:default": ["@platforms//:incompatible"],  # This would error out if say "bazel build -c dbg otel_sdk_r" is used.
    }),
    visibility = ["//visibility:private"],
) for (otel_sdk_binary, otel_sdk_config_name) in [
    ("otel_sdk_r", ":release"),
    ("otel_sdk_d", ":debug"),
    ("otel_sdk_rd", ":reldeb"),
]]

otel_cc_library(
    name = "api_sdk_includes",
    srcs = [
        "all_api_includes.cc",
        "all_sdk_includes.cc",
    ],
    target_compatible_with = select({
        # To compile you need `--//:with_dll=true` on the command line
        "with_dll_enabled": None,
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    # Almost all headers are included here, such that the compiler can notice the __declspec(dllexport) members.
    visibility = ["//visibility:private"],
    deps = ["otel_sdk_deps"],
)

# Conveniently place all Open Telemetry C++ dependencies required to build otel_sdk
[otel_cc_shared_library(
    name = otel_sdk_binary,
    # Force generation of .pdb file for for opt builds
    features = select({
        "@platforms//os:windows": ["generate_pdb_file"],
        "//conditions:default": None,
    }),
    target_compatible_with = select({
        # To compile you need `--//:with_dll=true` on the command line
        "with_dll_enabled": None,
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:private"],
    deps = [
        otel_sdk_binary + "_restrict_compilation_mode",
        "api_sdk_includes",
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
[otel_cc_import(
    name = otel_sdk_binary + "_dll",  #"_import",
    data = select({
        "@platforms//os:windows": [otel_sdk_binary + "_pdb_file"],
        "//conditions:default": None,
    }),
    interface_library = otel_sdk_binary + "_lib_file",
    shared_library = otel_sdk_binary,
    visibility = ["//visibility:public"],
    deps = [
        "//api:headers",
        "//exporters/elasticsearch:headers",
        "//exporters/memory:headers",
        "//exporters/ostream:headers",
        "//exporters/otlp:headers",
        "//exporters/prometheus:headers",
        "//exporters/zipkin:headers",
        "//ext:headers",
        "//sdk:headers",
    ] + select({
        "@platforms//os:windows": [
            "//exporters/etw:headers",
        ],
        "//conditions:default": [],
    }),
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

otel_cc_test(
    name = "dll_test",
    srcs = ["dll_test.cc"],
    deps = ["dll"],
)

pkg_files(
    name = "otel_sdk_header_files",
    srcs = [
        "//api:header_files",
        "//exporters/elasticsearch:header_files",
        "//exporters/memory:header_files",
        "//exporters/ostream:header_files",
        "//exporters/otlp:header_files",
        "//exporters/prometheus:header_files",
        "//exporters/zipkin:header_files",
        "//ext:header_files",
        "//sdk:header_files",
    ] + select({
        "@platforms//os:windows": [
            "//exporters/etw:header_files",
        ],
        "//conditions:default": [],
    }),
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
    ] + select({
        "@platforms//os:windows": [otel_sdk_binary + "_pdb_file"],
        "//conditions:default": [],
    }),
) for (otel_sdk_binary, compilation_mode) in [
    ("otel_sdk_r", "opt"),
    ("otel_sdk_d", "dbg"),
    ("otel_sdk_rd", "fastbuild"),
]]

# Same as above, but for the binaries
[force_compilation_mode(
    name = otel_sdk_binary + "_src_bundle" + "_force",
    compilation_mode = compilation_mode,
    data = [otel_sdk_binary + "_make_src_bundle"],
) for (otel_sdk_binary, compilation_mode) in [
    ("otel_sdk_r", "opt"),
    ("otel_sdk_d", "dbg"),
    ("otel_sdk_rd", "fastbuild"),
]]

# Collect all sources in a .src.zip bundle using sentry-cli - https://docs.sentry.io/product/cli/dif/
[run_binary(
    name = otel_sdk_binary + "_make_src_bundle_windows",
    srcs = [otel_sdk_binary + "_pdb_file"],
    outs = [otel_sdk_binary + ".src.zip"],
    args = [
        "debug-files",
        "bundle-sources",
        "$(location " + otel_sdk_binary + "_pdb_file" + ")",
    ],
    tags = ["manual"],
    tool = "@multitool//tools/sentry-cli",
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

[run_binary(
    name = otel_sdk_binary + "_make_src_bundle_non_windows",
    srcs = [otel_sdk_binary + "_lib_file"],
    outs = ["lib" + otel_sdk_binary + ".src.zip"],
    args = [
        "debug-files",
        "bundle-sources",
        "$(location " + otel_sdk_binary + "_lib_file" + ")",
    ],
    tags = [
        "manual",
        # TODO - Fix this to work in the sandbox, by using sentry-cli's -o folder
        "no-sandbox",
    ],
    tool = "@multitool//tools/sentry-cli",
) for otel_sdk_binary in [
    "otel_sdk_r",
    "otel_sdk_d",
    "otel_sdk_rd",
]]

[alias(
    name = otel_sdk_binary + "_make_src_bundle",
    actual = select({
        "@platforms//os:windows": otel_sdk_binary + "_make_src_bundle_windows",
        "//conditions:default": otel_sdk_binary + "_make_src_bundle_non_windows",
    }),
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
        "otel_sdk_d_lib_files",
        "otel_sdk_d_src_bundle",
        "otel_sdk_header_files",
        "otel_sdk_r_lib_files",
        "otel_sdk_r_src_bundle",
        "otel_sdk_rd_lib_files",
        "otel_sdk_rd_src_bundle",
    ],
)

# On windows we have .dll files in bin/, and import .lib files in lib/
# On linux/mac we have only .so/.dylib files in lib/ only.
pkg_filegroup(
    name = "otel_sdk_files_windows",
    srcs = [
        "otel_sdk_d_bin_files",
        "otel_sdk_r_bin_files",
        "otel_sdk_rd_bin_files",
    ],
)

pkg_zip(
    name = "otel_sdk_zip",
    srcs = ["otel_sdk_files"] + select({
        "@platforms//os:windows": ["otel_sdk_files_windows"],
        "//conditions:default": [],
    }),
    out = "otel_sdk.zip",
    tags = ["manual"],
)

# This would copy the file from the output build folder to the source folder (this directory)
write_source_file(
    name = "make_otel_sdk",
    in_file = "otel_sdk.zip",
    out_file = "otel_sdk.zip",
    tags = ["manual"],
)

[otel_cc_binary(
    name = "dll_deps_update_binary_" + os,
    srcs = ["dll_deps_update.cc"],
    data = ["otel_sdk_all_deps_" + os],
    local_defines = ['DEPS_FILE=\\"$(rlocationpath otel_sdk_all_deps_' + os + ')\\"'],
    deps = ["@bazel_tools//tools/cpp/runfiles"],
) for os in [
    "non_windows",
    "windows",
]]

[run_binary(
    name = "dll_deps_update_run_" + os,
    srcs = [":otel_sdk_all_deps_" + os],
    outs = ["dll_deps_generated_internally_" + os + ".bzl"],
    args = ["$(location dll_deps_generated_internally_" + os + ".bzl)"],
    tool = "dll_deps_update_binary_" + os,
) for os in [
    "non_windows",
    "windows",
]]

# To update the dll_deps_generated.bzl files, do this:
#    bazel run dll_deps_update_windows
#    bazel run dll_deps_update_non_windows
[write_source_file(
    name = "dll_deps_update_" + os,
    in_file = "dll_deps_generated_internally_" + os + ".bzl",
    # KLUDGE: Append repo.name() when this rule is ran outside of this repo, then it would be `otel_sdk+`
    # Having it this way, it'll allow dll_deps_generated_windows.bzl to be visible again.
    # When this target is used from another repo, then the "dll_deps_generated_windows.bzl" file can't be found
    out_file = repo_name().replace('+','_') + "dll_deps_generated_" + os + ".bzl",
) for os in [
    "non_windows",
    "windows",
]]

native_binary(
    name = "perses",
    src = "@multitool//tools/perses",
)

native_binary(
    name = "percli",
    src = "@multitool//tools/percli",
)

platform(
    name = "x64_windows-clang-cl",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
        "@bazel_tools//tools/cpp:clang-cl",
    ],
)
