load("@otel_sdk//:dll_deps_generated_non_windows.bzl", dll_deps_non_windows = "DLL_DEPS")
load("@otel_sdk//:dll_deps_generated_windows.bzl", dll_deps_windows = "DLL_DEPS")

# Filter libs that were compiled into the the otel_sdk.dll already
def _filter_libs(deps):
    """ Removes references to the api/sdk/exporters/ext static libraries """
    filtered_dll_deps = []

    #dll_deps = dll_deps_windows
    for dep in deps:
        label = native.package_relative_label(dep)
        if not label in dll_deps_windows:
            filtered_dll_deps.append(dep)
    return filtered_dll_deps

def dll_deps(deps):
    """ When building with --//:with_dll=true replaces the references to the api/sdk/exporters/ext static libraries with the single //:dll shared library """
    return select({
        "@otel_sdk//:with_dll_enabled": ["@otel_sdk//:dll"] + _filter_libs(deps),
        "//conditions:default": deps,
    })

force_compilation_mode = rule(
    implementation = lambda ctx: DefaultInfo(files = depset(
        transitive = [depset(transitive = [x[DefaultInfo].files]) for x in ctx.attr.data],
    )),
    attrs = {
        "compilation_mode": attr.string(mandatory = True),
        "data": attr.label_list(
            mandatory = True,
            cfg = transition(
                implementation = lambda settings, attr: {
                    "//command_line_option:compilation_mode": attr.compilation_mode,
                },
                inputs = [],
                outputs = ["//command_line_option:compilation_mode"],
            ),
        ),
        # This attribute is required to use starlark transitions. It allows
        # allowlisting usage of this rule. For more information, see
        # https://docs.bazel.build/versions/master/skylark/config.html#user-defined-transitions
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
)
