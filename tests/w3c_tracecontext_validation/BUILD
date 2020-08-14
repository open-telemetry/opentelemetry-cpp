package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tracecontext_validation",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "ext/zpages",
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//tests"
        "@github_nlohmann_json//:json",
    ],
)
