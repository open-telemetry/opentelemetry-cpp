# Very naive implementation
# TODO: Scan the deps for known targets included in the otel_sdk, and remove them, while leaving the rest. This would avoid the concatenation needed.
def dll_deps(deps):
    """Selects proper dll dependencies"""
    return select({
        "//:with_dll_enabled": ["//:dll"],
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
