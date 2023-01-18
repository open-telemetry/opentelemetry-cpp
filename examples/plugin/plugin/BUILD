load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "example_plugin.so",
    srcs = [
        "factory_impl.cc",
        "tracer.cc",
        "tracer.h",
    ],
    linkshared = 1,
    deps = dll_deps([
        "//api",
    ]),
)
