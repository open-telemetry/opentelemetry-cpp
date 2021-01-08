package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http_client_curl",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "src/http/client/curl",
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//sdk/src/common:random",
    ],
)
