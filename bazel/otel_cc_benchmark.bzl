def otel_cc_benchmark(name, srcs, deps):
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
    """

    # This is the benchmark as a binary, it can be run manually, and is used
    # to generate the _result below.
    native.cc_binary(
        name = name,
        srcs = srcs,
        deps = deps + ["@com_github_google_benchmark//:benchmark"],
        tags = ["manual"],
    )

    # The result of running the benchmark, captured into a text file.
    native.genrule(
        name = name + "_result",
        outs = [name + "_result.txt"],
        tools = [":" + name],
        tags = ["benchmark_result", "manual"],
        testonly = True,
        cmd = "$(location :" + name + (") --benchmark_color=false --benchmark_min_time=.1 &> $@"),
    )

    # This is run as part of "bazel test ..." to smoke-test benchmarks. It's
    # meant to complete quickly rather than get accurate results.
    native.cc_test(
        name = name + "_smoketest",
        srcs = srcs,
        deps = deps + ["@com_github_google_benchmark//:benchmark"],
        args = ["--benchmark_min_time=0"],
        tags = ["benchmark"],
    )
