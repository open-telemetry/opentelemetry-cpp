load("//:dll_deps.bzl", "dll_deps")

cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
    ]) + [
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
)
