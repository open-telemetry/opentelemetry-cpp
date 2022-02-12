cc_binary(
    name = "example_http_client",
    srcs = [
        "client.cc",
        "tracer_common.h",
    ],
    copts = [
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
    tags = ["ostream"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//ext/src/http/client:http_client_factory",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_http_server",
    srcs = [
        "server.cc",
        "server.h",
        "tracer_common.h",
    ],
    tags = ["ostream"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//sdk/src/trace",
    ],
)
