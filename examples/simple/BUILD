cc_library(
    name = "foo_library",
    srcs = [
        "foo_library/foo_library.cc",
        "foo_library/foo_library.h",
    ],
    linkstatic = True,
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
