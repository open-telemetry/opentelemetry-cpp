cc_test(
    name = "curl_http_test",
    srcs = [
        "curl_http_test.cc",
    ],
    deps = [
        "//ext:headers",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
        "@curl",
    ],
    # TODO: Move copts/linkopts for static CURL usage into shared bzl file.
    copts = [
        "-DCURL_STATICLIB",
    ],
    linkopts = select({
      "//bazel:windows": [ 
          "-DEFAULTLIB:advapi32.lib",
          "-DEFAULTLIB:crypt32.lib",
          "-DEFAULTLIB:Normaliz.lib",
      ],
      "//conditions:default": [],
    }),
    
)
