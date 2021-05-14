# Building opentelemetry-cpp

[CMake](https://cmake.org/) and [Bazel](https://bazel.build) are the official
build systems for opentelemetry-cpp.

## Build instructions using CMake

### Prerequisites

- A supported platform (e.g. Windows, macOS or Linux). Refer to [Platforms
  Supported](./README.md#supported-development-platforms) for more information.
- A compatible C++ compiler supporting at least C++11. Major compilers are
  supported. Refer to [Supported Compilers](./README.md#supported-c-versions)
  for more information.
- [Git](https://git-scm.com/) for fetching opentelemetry-cpp source code from
  repository. To install Git, consult the [Set up
  Git](https://help.github.com/articles/set-up-git/) guide on GitHub.
- [CMake](https://cmake.org/) for building opentelemetry-cpp API, SDK with their
  unittests. We use CMake version 3.15.2 in our build system. To install CMake,
  consult the [Installing CMake](https://cmake.org/install/) guide.
- [GoogleTest](https://github.com/google/googletest) framework to build and run
  the unittests. We use GoogleTest version 1.10.0 in our build system. To
  install GoogleTest, consult the [GoogleTest Build
  Instructions](https://github.com/google/googletest/blob/master/googletest/README.md#generic-build-instructions).
- Apart from above core requirements, the Exporters and Propagators have their
  build dependencies which are not covered here. E.g, Otlp Exporter needs
  grpc/protobuf library, Zipkin exporter needs nlohmann-json and libcurl, ETW
  exporter need nlohmann-json to build. This is covered in the build
  instructions for each of these components.

### Building as Standalone CMake Project

1. Getting the opentelementry-cpp source:

   ```console
   # Change to the directory where you want to create the code repository
   $ cd ~
   $ mkdir source && cd source
   $ git clone --recursive https://github.com/open-telemetry/opentelemetry-cpp
   Cloning into 'opentelemetry-cpp'...
   ...
   Resolving deltas: 100% (3225/3225), done.
   $
   ```

2. Navigate to the repository cloned above, and create the `CMake` build
   configuration.

   ```console
   $ cd opentelemetry-cpp
   $ mkdir build && cd build
   $ cmake ..
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

   - `-DWITH_OTLP=ON` : To enable building Otlp exporter.
   - `-DWITH_PROMETHEUS=ON` : To enable building prometheus exporter.

3. Once build configuration is created, build the CMake targets - this includes
   building SDKs, and building unittests for API and SDK. Note that since API is
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
   $ cmake --install . --config Debug --prefix /<install_root>/
   -- Installing: /<install-root>/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-config.cmake
   -- Installing: /<install-root>/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-config-version.cmake
   ...
   -- Installing: /<install-root>/include/opentelemetry//ext/zpages/static/tracez_index.h
   -- Installing: /<install-root>/include/opentelemetry//ext/zpages/static/tracez_style.h
   -- Installing: /<install-root>/include/opentelemetry//ext/zpages/threadsafe_span_data.h
   -- Installing: /<install-root>/lib/libopentelemetry_zpages.a
   $
   ```

### Incorporating Into An Existing CMake Project

To use the library from a CMake project, you can locate it directly with
 `find_package` and use the imported targets from generated package
 configurations. As of now, this will import targets for both API and SDK. In
 future, there may be separate packages for API and SDK which can be installed
 and imported separtely according to need.

```cmake
# CMakeLists.txt
find_package(opentelemetry-cpp CONFIG REQUIRED)
...
target_include_directories(foo PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
target_link_libraries(foo PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES})
```

## Build instructions using Bazel

TBD
