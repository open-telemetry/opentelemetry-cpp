package(default_visibility = ["//visibility:public"])

cc_library(
    name = "common_foo_library",
    srcs = [
        "foo_library.cc",
    ],
    hdrs = [
        "foo_library.h",
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//api",
        "//sdk:headers",
    ],
)
