cc_binary(
    name = "validate",
    srcs = [
        "tracecontext_validate.cc",
    ],
    linkopts = ["-lcurl"],
    deps = [
        "//api",
        "//ext/src/tracecontext_validation",
        "//sdk/src/trace",
    ],
)