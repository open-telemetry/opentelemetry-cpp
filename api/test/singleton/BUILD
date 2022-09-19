# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

DEFAULT_WIN_COPTS = [
]

# gcc and clang, assumed to be used on this platform
DEFAULT_NOWIN_COPTS = [
    "-fvisibility=default",
]

HIDDEN_WIN_COPTS = [
]

# gcc and clang, assumed to be used on this platform
HIDDEN_NOWIN_COPTS = [
    "-fvisibility=hidden",
]

cc_library(
    name = "component_a",
    srcs = [
        "component_a.cc",
    ],
    hdrs = [
        "component_a.h",
    ],
    linkstatic = True,
    deps = [
        "//api",
    ],
)

cc_library(
    name = "component_b",
    srcs = [
        "component_b.cc",
    ],
    hdrs = [
        "component_b.h",
    ],
    linkstatic = True,
    deps = [
        "//api",
    ],
)

cc_library(
    name = "component_c",
    srcs = [
        "component_c.cc",
    ],
    hdrs = [
        "component_c.h",
    ],
    copts = select({
        "//bazel:windows": DEFAULT_WIN_COPTS,
        "//conditions:default": DEFAULT_NOWIN_COPTS,
    }),
    linkstatic = False,
    deps = [
        "//api",
    ],
)

cc_library(
    name = "component_d",
    srcs = [
        "component_d.cc",
    ],
    hdrs = [
        "component_d.h",
    ],
    copts = select({
        "//bazel:windows": HIDDEN_WIN_COPTS,
        "//conditions:default": HIDDEN_NOWIN_COPTS,
    }),
    linkstatic = False,
    deps = [
        "//api",
    ],
)

cc_library(
    name = "component_e",
    srcs = [
        "component_e.cc",
    ],
    hdrs = [
        "component_e.h",
    ],
    copts = select({
        "//bazel:windows": DEFAULT_WIN_COPTS,
        "//conditions:default": DEFAULT_NOWIN_COPTS,
    }),
    linkstatic = False,
    deps = [
        "//api",
    ],
)

cc_library(
    name = "component_f",
    srcs = [
        "component_f.cc",
    ],
    hdrs = [
        "component_f.h",
    ],
    copts = select({
        "//bazel:windows": HIDDEN_WIN_COPTS,
        "//conditions:default": HIDDEN_NOWIN_COPTS,
    }),
    linkstatic = False,
    deps = [
        "//api",
    ],
)

cc_test(
    name = "singleton_test",
    srcs = [
        "singleton_test.cc",
    ],
    linkstatic = False,
    tags = [
        "api",
        "test",
    ],
    deps = [
        "component_a",
        "component_b",
        "component_c",
        "component_d",
        "component_e",
        "component_f",
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
