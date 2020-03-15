[cc_test(
    name = f[:-3],
    srcs = [f],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
) for f in glob(["*_test.cc"])]
