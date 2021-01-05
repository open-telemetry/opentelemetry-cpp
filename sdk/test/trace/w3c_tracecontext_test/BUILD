cc_binary(
    name = "w3c_tracecontext_test",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//ext:headers",
        "//sdk/src/trace",
        "@curl",
	"@github_nlohmann_json//:json",
    ],
)
