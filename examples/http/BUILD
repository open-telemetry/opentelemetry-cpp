cc_binary(
    name = "example_http_client",
    srcs = [
        "tracer_common.hpp",
        "client.cc",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
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
)

cc_binary(
    name = "example_http_server",
    srcs = [
        "server.cc",
        "tracer_common.hpp",
        "server.hpp",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
