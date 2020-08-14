package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "tracecontext_validation",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "ext/http/server",
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//tests"
        "@github_nlohmann_json//:json",
    ],
)