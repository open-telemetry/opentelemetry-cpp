package(default_visibility = ["//visibility:public"])

cc_library(
    name = "catch2",
    hdrs = [
        "include/opentelemetry/3rd_party/catch.hpp",
    ],
    strip_include_prefix = "include",
)

cc_library(
    name = "catch2_with_main",
    srcs = [
        "catch2_main.cpp",
    ],
    deps = [
      ":catch2",
    ],
)
