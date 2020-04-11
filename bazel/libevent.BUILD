load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")


filegroup(
    name = "srcs", 
    srcs = glob(["**"]), 
)

cmake_external(
    name = "libevent",
    cache_entries = {
      "CMAKE_BUILD_TYPE": "Release",
      "CMAKE_POSITION_INDEPENDENT_CODE" : "on",
      "BUILD_SHARED_LIBS": "off",
      "BUILD_STATIC_LIBS": "on",
      "EVENT__DISABLE_OPENSSL": "on",
      "EVENT__DISABLE_REGRESS": "on",
      "EVENT__DISABLE_TESTS": "on",
    },
    lib_source = ":srcs",
    static_libraries = ["libevent.a"],
    visibility = ["//visibility:public"],
)
