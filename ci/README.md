# CI Scripts

`./ci/do_ci.sh` is the main build, test, and validation script used by CI
and local development.

## Usage

Run targets directly inside the
[devcontainer](../CONTRIBUTING.md#devcontainer-setup-for-project):

```sh
./ci/do_ci.sh <target>
```

Run targets in Docker:

```sh
./ci/run_docker.sh ./ci/do_ci.sh <target>
```

You can also run `./ci/run_docker.sh` with no arguments to open an interactive
Docker shell and invoke targets manually.

## Examples

### Format Checks

```sh
./ci/do_ci.sh format
markdownlint .
shellcheck --severity=error <path to shell script>.sh
```

### Testing

```sh
./ci/do_ci.sh cmake.maintainer.sync.test
./ci/do_ci.sh cmake.c++20.stl.test
./ci/run_docker.sh ./ci/do_ci.sh bazel.test
```

## Build configuration

The script accepts several environment variables to configure the build:

```sh
BUILD_DIR=$HOME/build
BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel
CXX_STANDARD=14|17|20|23
BUILD_SHARED_LIBS=ON|OFF
OTELCPP_CMAKE_BUILD_ARGS=<cmake-build-args>
OTELCPP_CMAKE_CACHE_FILE=<cache-file-name>
```

`OTELCPP_CMAKE_CACHE_FILE` should be the basename of a cache file from
`test_common/cmake`, for example `all-options-abiv1-preview.cmake` or
`all-options-abiv2.cmake`.

`OTELCPP_CMAKE_BUILD_ARGS` overrides the default `cmake --build` arguments
(`--parallel`). For example, to limit parallelism and enable verbose output:

```sh
OTELCPP_CMAKE_BUILD_ARGS="--parallel 2 --verbose" ./ci/do_ci.sh code.coverage
```

## Targets

### Formatting and coverage

* `format`: Run formatting tools (`clang-format`, `cmake-format`,
  `buildifier`) and fail if files change.
* `code.coverage`: Build with coverage flags and generate `coverage.info`.
* `cmake.clang_tidy.test`: Build with `clang-tidy` enabled and emit a build
  log.

### Maintainer CMake builds

These targets use the `all-options-*` cache files in `test_common/cmake` and
build all CMake components with preview options enabled.

* `cmake.maintainer.sync.test`: Maintainer mode ABI v1 build with synchronous
  export and tests.
* `cmake.maintainer.async.test`: Maintainer mode ABI v1 build with async export
  and tests.
* `cmake.maintainer.abiv2.test`: Maintainer mode ABI v2 build and tests.
* `cmake.maintainer.yaml.test`: Maintainer mode build with YAML configuration.

### CMake language-standard matrix

These tests build the core API and SDK components in maintainer mode.

* `cmake.c++14.test`: C++14, WITH_STL=OFF
* `cmake.c++17.test`: C++17, WITH_STL=OFF
* `cmake.c++20.test`: C++20, WITH_STL=OFF
* `cmake.c++23.test`: C++23, WITH_STL=OFF
* `cmake.c++14.stl.test`: C++14, WITH_STL=CXX14
* `cmake.c++17.stl.test`: C++17, WITH_STL=CXX17
* `cmake.c++20.stl.test`: C++20, WITH_STL=CXX20
* `cmake.c++23.stl.test`: C++23, WITH_STL=CXX23

### CMake feature and packaging tests

* `cmake.with_async_export.test`: Standard CMake build with async export
  enabled.
* `cmake.opentracing_shim.test`: OpenTracing shim build and tests.
* `cmake.opentracing_shim.install.test`: OpenTracing shim install validation
  test.
* `cmake.exporter.otprotocol.test`: OTLP exporter build and tests.
* `cmake.exporter.otprotocol.shared_libs.with_static_grpc.test`: OTLP exporter
  test with shared libraries enabled.
* `cmake.exporter.otprotocol.with_async_export.test`: OTLP exporter test with
  async export enabled.
* `cmake.w3c.trace-context.build-server`: Build the W3C trace-context test
  server.
* `cmake.do_not_install.test`: Build and test with installation disabled.
* `cmake.install.test`: Install-tree validation and downstream CMake package
  test.
* `cmake.fetch_content.test`: Validate building the project via
  `FetchContent`.
* `cmake.test_example_plugin`: Build and load-test the example plugin.

### Bazel targets

* `bazel.test`: Build and test all Bazel targets.
* `bazel.with_async_export.test`: Build and test with async export enabled.
* `bazel.macos.test`: Build and test using the macOS Bazel configuration.
* `bazel.legacy.test`: Bazel legacy compatibility test subset.
* `bazel.noexcept`: Build and test with exceptions disabled where supported.
* `bazel.nortti`: Build and test with RTTI disabled where supported.
* `bazel.asan`: Run Bazel tests with AddressSanitizer.
* `bazel.tsan`: Run Bazel tests with ThreadSanitizer.
* `bazel.valgrind`: Run Bazel tests under Valgrind.

### Benchmarks

* `benchmark`: Build benchmark binaries and collect result artifacts.
