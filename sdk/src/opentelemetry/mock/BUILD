package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tracer",
    hdrs = [
        "tracer.h",
        "span.h",
    ],
    srcs = [
        "tracer.cc",
        "span.cc",
    ],
    deps = [
        "//api",
    ],
)

cc_library(
    name = "plugin",
    srcs = [
        "factory_impl.h",
        "factory_impl.cc",
    ],
    deps = [
        ":tracer",
    ],
)
