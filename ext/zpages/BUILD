package(default_visibility = ["//visibility:public"])

cc_library(
    name = "zpages",
    srcs = glob(["src/**/*.cc"]),
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//ext/http/server:headers",
        "//sdk:headers",
        "@github_nlohmann_json//:json",
    ],
)

