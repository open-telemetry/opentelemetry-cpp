cc_library(
    name = "foo_library",
    srcs = [
        "foo_library/foo_library.cc",
    ],
    hdrs = [
        "foo_library/foo_library.h",
    ],
    deps = [
        "//api",
        "//sdk:headers",
    ],
)

cc_binary(
    name = "example_zipkin",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":foo_library",
       "//api",
        "//exporters/zipkin:zipkin_exporter",
        "//sdk/src/trace",
    ],
)
