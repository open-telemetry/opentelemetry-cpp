cc_binary(
    name = "example_http_client",
    srcs = [
        "client.cc",
        "tracer_common.hpp",
    ],
    # TODO: Move copts/linkopts for static CURL usage into shared bzl file.
    copts = [
        "-DCURL_STATICLIB",
        "-DWITH_CURL",
    ],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
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
        "server.hpp",
        "tracer_common.hpp",
    ],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//sdk/src/trace",
    ],
)
