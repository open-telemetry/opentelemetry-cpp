cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    # TODO: Move copts/linkopts for static CURL usage into shared bzl file.
    copts = [
        "-DCURL_STATICLIB",
        "-DWITH_CURL",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
            "-DEFAULTLIB:Normaliz.lib",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
