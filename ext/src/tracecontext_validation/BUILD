package(default_visibility = ["//visibility:public"])

cc_library(
    name = "validation",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "ext/tracecontext_validation",
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
    ],
)
