cc_binary(
    name = "w3c_tracecontext_test",
    srcs = [
        "main.cc",
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
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//sdk/src/trace",
        "@curl",
        "@github_nlohmann_json//:json",
    ],
)
