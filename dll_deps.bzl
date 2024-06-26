load("dll_deps_generated_non_windows.bzl", dll_deps_non_windows = "DLL_DEPS")
load("dll_deps_generated_windows.bzl", dll_deps_windows = "DLL_DEPS")

def _absolute_label(label):
    """ returns the absolute path to a label string """
    # lifted from https://stackoverflow.com/a/66705640/18406610
    if label.startswith("@") or label.startswith("/"):
        return label
    if label.startswith(":"):
        return native.repository_name() + "//" + native.package_name() + label
    return native.repository_name() + "//" + native.package_name() + ":" + label

# Filter libs that were compiled into the the otel_sdk.dll already
def _filter_libs(deps):
    """ Removes references to the api/sdk/exporters/ext static libraries """
    filtered_dll_deps = []
#    dll_deps = select({
#        "@platforms//os:windows": dll_deps_windows,
#        "//conditions:default": dll_deps_non_windows,
#    })
    dll_deps = dll_deps_windows
    for dep in deps:
        label = Label(_absolute_label(dep))
        if not label in dll_deps:
            filtered_dll_deps.append(dep)
    return filtered_dll_deps

def dll_deps(deps):
    """ When building with --//:with_dll=true replaces the references to the api/sdk/exporters/ext static libraries with the single //:dll shared library """
    return select({
        "//:with_dll_enabled": ["//:dll"] + _filter_libs(deps),
        "//conditions:default": deps
    })

force_compilation_mode = rule(
    implementation = lambda ctx: 
        DefaultInfo( files = depset( 
            transitive = [ depset( transitive = [x[DefaultInfo].files] ) for x in ctx.attr.data ]
        )),
    attrs = {
        "compilation_mode": attr.string(mandatory = True),
        "data": attr.label_list(
            mandatory = True,
            cfg = transition(
                implementation = lambda settings, attr: {
                    "//command_line_option:compilation_mode": attr.compilation_mode
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
    }
)

def avoid_dll_lock():
    """
    Dummy target to avoid dll locking when the otel_sdk.dll has to be copied
    simualtenously into the same output folder. This ensures the copying
    happens once, through this target, as others depends on it

    Example, place this somewhere in your BUILD file:

      avoid_dll_lock()
    """
    pass
    #native.cc_library(
    #    name = "avoid_dll_lock",
    #    deps = ["//:dll"]
    #)
