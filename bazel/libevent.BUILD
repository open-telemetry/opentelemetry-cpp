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
    static_libraries = select({
      "@io_opentelemetry_cpp//bazel:windows": ["libevent.lib"],
      "//conditions:default": ["libevent.a"],
    }),
    make_commands = select({
      "@io_opentelemetry_cpp//bazel:windows": ["MSBuild.exe INSTALL.vcxproj"],
      "//conditions:default" : None,
    }),
    visibility = ["//visibility:public"],
)
