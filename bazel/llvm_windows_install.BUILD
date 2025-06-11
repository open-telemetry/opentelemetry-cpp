ASAN_FOLDER = "lib/clang/20/lib/windows/"

cc_import(
    name = "asan1",
    interface_library = ASAN_FOLDER + "clang_rt.asan_dynamic-x86_64.lib",
    shared_library = ASAN_FOLDER + "clang_rt.asan_dynamic-x86_64.dll",
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)

cc_import(
    name = "asan2",
    static_library = ASAN_FOLDER + "clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "asan",
    deps = ["asan1", "asan2"],
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)