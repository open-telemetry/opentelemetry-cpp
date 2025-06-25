LIBCLANG_DIR = "lib/clang/20/"
SHARE_DIR = LIBCLANG_DIR + "share/"
WIN_DIR = LIBCLANG_DIR + "lib/windows/"

cc_import(
    name = "asan1",
    interface_library = WIN_DIR + "clang_rt.asan_dynamic-x86_64.lib",
    shared_library = WIN_DIR + "clang_rt.asan_dynamic-x86_64.dll",
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)

cc_import(
    name = "asan2",
    static_library = WIN_DIR + "clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)

exports_files(
    SHARE_DIR + "asan_ignorelist.txt"
)

cc_library(
    name = "asan",
    deps = ["asan1", "asan2"],
    #alwayslink = 1,
    visibility = ["//visibility:public"],
)
