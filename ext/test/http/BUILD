cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//ext:headers",
        "//ext/src/http/client:http_client_factory",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
