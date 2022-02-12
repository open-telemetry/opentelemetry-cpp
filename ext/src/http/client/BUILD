package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http_client_factory",
    srcs = [
        "http_client_factory.cc",
    ],
    include_prefix = "src/http/client",
    deps = [
        "//api",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//ext/src/http/client/nosend:http_client_nosend",
        "//sdk:headers",
    ],
)
