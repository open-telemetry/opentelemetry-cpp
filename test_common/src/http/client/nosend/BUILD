package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http_client_nosend",
    srcs = [
        "http_client_factory_nosend.cc",
        "http_client_nosend.cc",
    ],
    include_prefix = "src/http/client/nosend",
    tags = [
        "test",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//test_common:headers",
        "@com_google_googletest//:gtest_main",
    ],
)
