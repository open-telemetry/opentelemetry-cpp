# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

def otel_cc_benchmark(name, srcs, deps, tags = [""], local_defines = []):
    """
    Creates targets for the benchmark and related targets.

    Example:

      otel_cc_benchmark(
          name = "foo_benchmark",
          srcs = ["foo_benchmark.cc"],
          deps = ["//bar"],
      )

    Creates:

      :foo_benchmark           (the benchmark binary)
      :foo_benchmark_result    (results from running the benchmark)
      :foo_benchmark_smoketest (a fast test that runs a single iteration)

    Args:
      name: name of the benchmark binary.
      srcs: sources of the benchmark.
      deps: dependencies of the benchmark.
      tags: extra tags for every generated target.
      local_defines: preprocessor defines for the benchmark sources only. They
        are not passed on to anything that depends on the generated targets,
        which is what lets the same source be built twice under different
        defines.
    """

    # This is the benchmark as a binary, it can be run manually, and is used
    # to generate the _result below.
    cc_binary(
        name = name,
        srcs = srcs,
        deps = deps + ["@com_github_google_benchmark//:benchmark"],
        tags = tags + ["manual"],
        defines = ["BAZEL_BUILD"],
        local_defines = local_defines,
    )

    # The result of running the benchmark, captured into a text file.
    native.genrule(
        name = name + "_result",
        outs = [name + "_result.json"],
        tools = [":" + name],
        tags = tags + ["benchmark_result", "manual"],
        testonly = True,
        cmd = "$(location :" + name + (") --benchmark_format=json --benchmark_color=false --benchmark_min_time=.1s > $@ 2>/dev/null"),
    )

    # This is run as part of "bazel test ..." to smoke-test benchmarks. It's
    # meant to complete quickly rather than get accurate results.
    cc_test(
        name = name + "_smoketest",
        srcs = srcs,
        deps = deps + ["@com_github_google_benchmark//:benchmark"],
        args = ["--benchmark_min_time=1x"],
        tags = tags + ["benchmark"],
        defines = ["BAZEL_BUILD"],
        local_defines = local_defines,
    )
