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
    ],
)

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
        "stdout_exporter.h",
    ],
    deps = [
        ":foo_library",
        "//api",
        "//sdk/src/trace",
    ],
)
