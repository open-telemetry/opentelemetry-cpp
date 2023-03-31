# Very naive implementation
# TODO: Scan the deps for known targets included in the otel_sdk, and remove them, while leaving the rest. This would avoid the concatenation needed.

load("dll_deps_generated.bzl", "DLL_DEPS")

def _absolute_label(label):
    """ returns the absolute path to a lebel string """
    # lifted from https://stackoverflow.com/a/66705640/18406610
    if label.startswith("@") or label.startswith("/"):
        return label
    if label.startswith(":"):
        return native.repository_name() + "//" + native.package_name() + label
    return native.repository_name() + "//" + native.package_name() + ":" + label

def _remove_static_libs(deps):
    """ Removes references to the api/sdk/exporters/ext static libraries """
    filtered_dll_deps = []
    for dep in deps:
        abs = _absolute_label(dep)
        label = Label(abs)
#       parts = str(label.package).split('/')
        if not label in DLL_DEPS:
            filtered_dll_deps.append(dep)
        # if "test" in parts:
        #     filtered_dll_deps.append(dep)
        # elif not parts[0] in ["api", "exporters", "sdk", "ext"]:
        #     filtered_dll_deps.append(dep)
    return filtered_dll_deps

def dll_deps(deps):
    """ When building with --//:with_dll=true replaces the references to the api/sdk/exporters/ext static libraries with the single //:dll shared library """
    return select({
        "//:with_dll_enabled": ["//:dll"] + _remove_static_libs(deps),
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

def dll_cc_binary(**kwargs):
    """ blah """
    for k,v in kwargs.items():
        print(k, v, type(k), type(v))
    native.cc_binary(**kwargs)
