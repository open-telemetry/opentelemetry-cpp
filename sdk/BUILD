package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sdk",
    srcs = glob(["src/**/*.cc"]),
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = ["//api"],
)
