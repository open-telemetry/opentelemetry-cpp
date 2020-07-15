package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "httpserver",
    srcs = [
        "main.cpp"
    ],
    linkopts = select({
    #    "//bazel:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    deps = [
        "//ext:headers",
#        "@github_nlohmann_json//:json",
    ],
)
