cc_test(
    name = "function_ref_test",
    srcs = [
        "function_ref_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = [
        "string_view_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "variant_test",
    srcs = [
        "variant_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "variant_test_dll_deps",
    local_defines = ["OPENTELEMETRY_API=__declspec(dllexport)"],
    linkshared = True,
    tags = [
        "dll",
    ],
    deps = [
        "//api",
    ]
)

cc_test(
    name = "variant_test_dll",
    srcs = [
        "variant_test.cc",
    ],
    tags = [
        "dll",
        "api",
        "test",
    ],
    deps = [
        "variant_test_dll_deps",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "unique_ptr_test",
    srcs = [
        "unique_ptr_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "utility_test",
    srcs = [
        "utility_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "span_test",
    srcs = [
        "span_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "shared_ptr_test",
    srcs = [
        "shared_ptr_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
