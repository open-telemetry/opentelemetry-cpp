cc_binary(
    name = "example_http_client",
    srcs = [
        "tracer_common.hpp",
        "client.cc",
    ],
    deps = [
        "//api",
        "//ext/src/http/client/curl:http_client_curl",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
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
