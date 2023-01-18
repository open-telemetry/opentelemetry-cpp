# Very naive implementation
# TODO: Scan the deps for known targets included in the otel_sdk, and remove them, while leaving the rest. This would avoid the concatenation needed.
def dll_deps(deps):
    return select({
        "//:with_dll_enabled": ["//:dll"],
        "//conditions:default": deps
    })

