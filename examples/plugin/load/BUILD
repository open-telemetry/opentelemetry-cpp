load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "load_plugin",
    srcs = [
        "main.cc",
    ],
    linkopts = [
        "-ldl",
    ],
    deps = dll_deps([
        "//api",
    ]),
)
