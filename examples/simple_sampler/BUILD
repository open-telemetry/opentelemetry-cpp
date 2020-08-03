cc_library(
    name = "testing_library",
    srcs = [
        "testing_library/testing_library.cc",
    ],
    hdrs = [
        "testing_library/testing_library.h",
    ],
    deps = [
        "//api",
    ],
)

cc_library(
    name = "stdout_exporter",
    srcs = [
        "stdout_exporter.cc",
    ],
    hdrs = [
        "stdout_exporter.h",
    ],
    deps = [
        "//api",
        "//sdk/src/trace",
    ],
)

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":testing_library",
        ":stdout_exporter",
        "//api",
        "//sdk/src/trace",
    ],
)
