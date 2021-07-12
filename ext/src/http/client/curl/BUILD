package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http_client_curl",
    srcs = [
        "http_client_curl.cc",
        "http_client_factory_curl.cc",
    ],
    copts = [
        "-DWITH_CURL",
    ],
    include_prefix = "src/http/client/curl",
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
            "-DEFAULTLIB:Ws2_32.lib",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//sdk/src/common:random",
        "@curl",
    ],
)
