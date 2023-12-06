# Building opentelemetry-cpp

[CMake](https://cmake.org/) and [Bazel](https://bazel.build) are the official
build systems for opentelemetry-cpp.

## Dependencies

You can link OpenTelemetry C++ SDK with libraries provided in
[dependencies.md](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/docs/dependencies.md)
(complete list of libraries with versions used in our CI can be found
[here](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/third_party_release)).

## Build instructions using CMake

### Prerequisites for CMake build

- A supported platform (e.g. Windows, macOS or Linux). Refer to [Platforms
  Supported](./README.md#supported-development-platforms) for more information.
- A compatible C++ compiler supporting at least C++14. Major compilers are
  supported. Refer to [Supported Compilers](./README.md#supported-c-versions)
  for more information.
- [Git](https://git-scm.com/) for fetching opentelemetry-cpp source code from
  repository. To install Git, consult the [Set up
  Git](https://help.github.com/articles/set-up-git/) guide on GitHub.
- [CMake](https://cmake.org/) for building opentelemetry-cpp API, SDK with their
  unittests. We use CMake version 3.15.2 in our build system. To install CMake,
  consult the [Installing CMake](https://cmake.org/install/) guide.
- [GoogleTest](https://github.com/google/googletest) framework to build and run
  the unittests. Refer to
  [third_party_release](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/third_party_release#L5)
  for version of GoogleTest used in CI. To install GoogleTest, consult the
  [GoogleTest Build
  Instructions](https://github.com/google/googletest/blob/master/googletest/README.md#generic-build-instructions).
- [Google Benchmark](https://github.com/google/benchmark) framework to build and
  run benchmark tests. Refer to
  [third_party_release](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/third_party_release#L4)
  for version of Benchmark used in CI. To install Benchmark, consult the
  [GoogleBenchmark Build
  Instructions](https://github.com/google/benchmark#installation).
- Apart from above core requirements, the Exporters and Propagators have their
  build dependencies which are not covered here. E.g, the OTLP Exporter needs
  grpc/protobuf library, the Zipkin exporter needs nlohmann-json and libcurl,
  the ETW exporter needs nlohmann-json to build. This is covered in the build
  instructions for each of these components.

### Building as standalone CMake Project

1. Getting the opentelemetry-cpp source with its submodules:

   ```console
   # Change to the directory where you want to create the code repository
   $ cd ~
   $ mkdir source && cd source && git clone --recurse-submodules https://github.com/open-telemetry/opentelemetry-cpp
   Cloning into 'opentelemetry-cpp'...
   ...
   Resolving deltas: 100% (3225/3225), done.
   $
   ```

2. Navigate to the repository cloned above, and create the `CMake` build
   configuration.

   ```console
   $ cd opentelemetry-cpp
   $ mkdir build && cd build && cmake ..
   -- The C compiler identification is GNU 9.3.0
   -- The CXX compiler identification is GNU 9.3.0
   ...
   -- Configuring done
   -- Generating done
   -- Build files have been written to: /home/<user>/source/opentelemetry-cpp/build
   $
   ```

   Some of the available cmake build variables we can use during cmake
   configuration:

   - `-DCMAKE_POSITION_INDEPENDENT_CODE=ON` : Please note that with default
     configuration, the code is compiled without `-fpic` option, so it is not
     suitable for inclusion in shared libraries. To enable the code for
     inclusion in shared libraries, this variable is used.
   - `-DBUILD_SHARED_LIBS=ON` : To build shared libraries for the targets.
      Please refer to note [below](#building-shared-libs-for-windows) for
      Windows DLL support.
   - `-DWITH_OTLP_GRPC=ON` : To enable building OTLP GRPC exporter.
   - `-DWITH_OTLP_HTTP=ON` : To enable building OTLP HTTP exporter.
   - `-DWITH_PROMETHEUS=ON` : To enable building prometheus exporter.

3. Once the build configuration is created, build the CMake targets - this
   includes building SDKs and unittests for API and SDK. Note that since API is
   header only library, no separate build is triggered for it.

   ```console
   $ cmake --build . --target all
   Scanning dependencies of target timestamp_test
   [  0%] Building CXX object api/test/core/CMakeFiles/timestamp_test.dir/timestamp_test.cc.o
   [  1%] Linking CXX executable timestamp_test
   ...
   Scanning dependencies of target w3c_tracecontext_test
   [ 99%] Building CXX object ext/test/w3c_tracecontext_test/CMakeFiles/w3c_tracecontext_test.dir/main.cc.o
   [100%] Linking CXX executable w3c_tracecontext_test
   [100%] Built target w3c_tracecontext_test
   $
   ```

4. Once CMake tests are built, run them with `ctest` command

   ```console
   $ ctest
   Test project /tmp/opentelemetry-cpp/build
        Start   1: trace.SystemTimestampTest.Construction
   ...
        Start 380: ext.http.urlparser.UrlParserTests.BasicTests
   ...
   100% tests passed, 0 tests failed out of 380
   $
   ```

5. Optionally install the header files for API, and generated targets and header
   files for SDK at custom/default install location.

   ```console
   $ cmake --install . --prefix /<install-root>/
   -- Installing: /<install-root>/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-config.cmake
   -- Installing: /<install-root>/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-config-version.cmake
   ...
   $
   ```

### Incorporating into an existing CMake Project

To use the library from a CMake project, you can locate it directly with
 `find_package` and use the imported targets from generated package
 configurations. As of now, this will import targets for both API and SDK. In
 future, there may be separate packages for API and SDK which can be installed
 and imported separately according to need.

```cmake
# CMakeLists.txt
find_package(opentelemetry-cpp CONFIG REQUIRED)
...
target_include_directories(foo PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
target_link_libraries(foo PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES})
```

## Build instructions using Bazel

NOTE: Experimental, and not supported for all the components. Make sure the
[GoogleTest](https://github.com/google/googletest) installation may fail if
there is a different version of googletest already installed in system-defined
path.

### Prerequisites for Bazel build

- A supported platform (e.g. Windows, macOS or Linux). Refer to [Platforms
Supported](./README.md#supported-development-platforms) for more information.
- A compatible C++ compiler supporting at least C++14. Major compilers are
supported. Refer to [Supported Compilers](./README.md#supported-c-versions) for
more information.
- [Git](https://git-scm.com/) for fetching opentelemetry-cpp source code from
repository. To install Git, consult the [Set up
Git](https://help.github.com/articles/set-up-git/) guide on GitHub.
- [Bazel](https://www.bazel.build/) for building opentelemetry-cpp API, SDK with
their unittests. We use 3.7.2 in our build system.

To install Bazel, consult the [Installing
Bazel](https://docs.bazel.build/versions/3.7.0/install.html) guide.

### Building as standalone Bazel Project

1. Getting the opentelemetry-cpp source:

   ```console
   # Change to the directory where you want to create the code repository
   $ cd ~
   $ mkdir source && cd source
   $ git clone https://github.com/open-telemetry/opentelemetry-cpp
   Cloning into 'opentelemetry-cpp'...
   ...
   Resolving deltas: 100% (3225/3225), done.
   $
   ```

2. Navigate to the repository cloned above, download the dependencies and build
   the source code:

   ```console
   $ cd opentelemetry-cpp
   $ bazel build //...
   bazel build -- //... -//exporters/otlp/... -//exporters/prometheus/...
   Extracting Bazel installation...
   Starting local Bazel server and connecting to it...
   INFO: Analyzed 121 targets (98 packages loaded, 3815 targets configured).
   INFO: Found 121 targets...
   INFO: From Compiling sdk/src/trace/tracer_context.cc:
   ...

   ```

3. Once Bazel tests are built, run them with `bazel test //...` command

   ```console
   $ bazel test //...
   ..
   $
   ```

4. The build artifacts will be located under `bazel-bin`

### Incorporating into an existing Bazel Project

- WORKSPACE file:

```console
http_archive(
    name = "io_opentelemetry_cpp",
    sha256 = "<sha256>",
    strip_prefix = "opentelemetry-cpp-1.0.1",
    urls = [
        "https://github.com/open-telemetry/opentelemetry-cpp/archive/refs/tags/v1.0.1.tar.gz"
    ],
)

# Load OpenTelemetry dependencies after load.
load("@io_opentelemetry_cpp//bazel:repository.bzl", "opentelemetry_cpp_deps")

opentelemetry_cpp_deps()

# (required after v1.8.0) Load extra dependencies required for OpenTelemetry
load("@io_opentelemetry_cpp//bazel:extra_deps.bzl", "opentelemetry_extra_deps")

opentelemetry_extra_deps()

# Load gRPC dependencies after load.
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

# Load extra gRPC dependencies due to https://github.com/grpc/grpc/issues/20511
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

```

- Component level BUILD file:

```console
cc_library(
   name = "<name>"
   ...
   deps = [
      "@io_opentelemetry_cpp//api",
      "@io_opentelemetry_cpp//exporters/otlp:otlp_exporter",
      "@io_opentelemetry_cpp//sdk/src/trace",
      ...
   ],
   ...
)
```

## Building shared libs for Windows

Windows DLL build is supported under **preview**. Please check the
[doc](./docs/build-as-dll.md) for more details.

## Generating binary packages

OpenTelemetry C++ supports generating platform specific binary packages from CMake
configuration. The packages generated through this mayn't be production ready,
and user may have to customize it further before using it as distribution.

- Linux : deb, rpm, tgz
- MacOS : tgz
- Windows : NuGet, zip

This requires platform specific package generators already installed. The package
generation can subsequently be enabled by using BUILD_PACKAGE option during cmake
configuration

   ```console
   $ cd opentelemetry-cpp
   $ mkdir build && cd build && cmake -DBUILD_PACKAGE=ON ..

   -- Package name: opentelemetry-cpp-1.8.1-ubuntu-20.04-x86_64.deb
   -- Configuring done
   -- Generating done
   ...
   $
   ```

Once build is complete as specified in [standalone build section](#building-as-standalone-cmake-project),
the package can be generated as below.

   ```console
   $ cpack -C debug
   CPack: Create package using DEB
   ...
   CPack: - package: /home/<user>/opentelemetry-cpp/build/opentelemetry-cpp-1.8.1-ubuntu-20.04-x86_64.deb generated.
   $
   ```

## Using Package Managers

If you are using [Conan](https://www.conan.io/) to manage your dependencies, add
[`opentelemetry-cpp/x.y.z`](https://conan.io/center/opentelemetry-cpp) to your
`conanfile`'s requires, where `x.y.z` is the release version you want to use.
Please file issues [here](https://github.com/conan-io/conan-center-index/issues)
if you experience problems with the packages.

If you are using [vcpkg](https://github.com/Microsoft/vcpkg/) on your project
for external dependencies, then you can install the [opentelemetry-cpp
package](https://github.com/microsoft/vcpkg/tree/master/ports/opentelemetry-cpp)
with `vcpkg install opentelemetry-cpp` and follow the then displayed
descriptions. Please see the vcpkg project for any issues regarding the
packaging.

If you are using [alpine linux](https://www.alpinelinux.org/) you can install
the [opentelemetry-cpp packages](https://pkgs.alpinelinux.org/packages?name=opentelemetry-cpp-*)
with `apk add -X http://dl-cdn.alpinelinux.org/alpine/edge/testing opentelemetry-cpp-dev`.

Please note, these packages are not officially provided and maintained by
OpenTelemetry C++ project, and are just listed here to consolidate all such
efforts for ease of developers.
