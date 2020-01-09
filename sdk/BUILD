package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sdk",
    deps = ["//api"],
    hdrs = glob(["include/**/*.h"]),
    srcs = glob(["src/**/*.cc"]),
    strip_include_prefix = "include",
)
