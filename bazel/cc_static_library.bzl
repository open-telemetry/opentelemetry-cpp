"""Provides a rule that outputs a monolithic static library."""

# Reference: https://gist.github.com/oquenchil/3f88a39876af2061f8aad6cdc9d7c045

load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

TOOLS_CPP_REPO = "@bazel_tools"

def _cc_static_library_impl(ctx):
    output_lib = ctx.actions.declare_file("{}.a".format(ctx.attr.name))
    output_flags = ctx.actions.declare_file("{}.link".format(ctx.attr.name))

    cc_toolchain = find_cpp_toolchain(ctx)

    # Aggregate linker inputs of all dependencies
    lib_sets = []
    for dep in ctx.attr.deps:
        lib_sets.append(dep[CcInfo].linking_context.linker_inputs)
    input_depset = depset(transitive = lib_sets)

    # Collect user link flags and make sure they are unique
    unique_flags = {}
    for inp in input_depset.to_list():
        unique_flags.update({
            flag: None
            for flag in inp.user_link_flags
        })
    link_flags = unique_flags.keys()

    # Collect static libraries
    libs = []
    for inp in input_depset.to_list():
        for lib in inp.libraries:
            if lib.pic_static_library:
                libs.append(lib.pic_static_library)
            elif lib.static_library:
                libs.append(lib.static_library)

    script_file = ctx.actions.declare_file("{}.mri".format(ctx.attr.name))
    commands = ["create {}".format(output_lib.path)]
    for lib in libs:
        commands.append("addlib {}".format(lib.path))
    commands.append("save")
    commands.append("end")
    ctx.actions.write(
        output = script_file,
        content = "\n".join(commands) + "\n",
    )

    ar_path = cc_toolchain.ar_executable
    # FIXME ar_executable returned llvm-lib.exe on my system, but we want llvm-ar.exe
    ar_path = ar_path.replace("llvm-lib.exe", "llvm-ar.exe")

    ctx.actions.run_shell(
        command = "\"{}\" -M < {}".format(ar_path, script_file.path),
        inputs = [script_file] + libs + cc_toolchain.all_files.to_list(),
        outputs = [output_lib],
        mnemonic = "ArMerge",
        progress_message = "Merging static library {}".format(output_lib.path),
    )
    ctx.actions.write(
        output = output_flags,
        content = "\n".join(link_flags) + "\n",
    )
    return [
        DefaultInfo(files = depset([output_flags, output_lib])),
    ]

cc_static_library = rule(
    implementation = _cc_static_library_impl,
    attrs = {
        "deps": attr.label_list(),
        "_cc_toolchain": attr.label(
            default = TOOLS_CPP_REPO + "//tools/cpp:current_cc_toolchain",
        ),
    },
    toolchains = [TOOLS_CPP_REPO + "//tools/cpp:toolchain_type"],
    incompatible_use_toolchain_transition = True,
)
