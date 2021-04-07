package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http_client_curl",
    srcs = [
        "http_client_curl.cc",
        "http_client_factory_curl.cc",
    ],
    include_prefix = "src/http/client/curl",
    # TODO: Move copts/linkopts for static CURL usage into shared bzl file.
    copts = [
        "-DCURL_STATICLIB",
        "-DWITH_CURL",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//sdk/src/common:random",
        "@curl",
    ],
)
