# OpenTelemetry as a single .dll

This branch builds a single `.dll`, combining both api + sdk parts, instead of being separate .dlls.

Why?
  - The `vcpkg` version of the package can build only static libraries, and in the future, if dynamic works it'll create .dll for each component: zlib, protobuf, etc.
  - The built-in `CMake` rules produce more than one .dll, and do not expose all of the `sdk` by default.

There are some critical issues with OpenTelemetry C++ in the original branch:
  - The static library version does not work across dlls using it - their own `singleton_test` fails.
  - The static library brings dozen of dependencies (zlib, protobuf, grpc, etc.)
  - This not only increases the distribution size (from 100mb to 2GB), it can also silently introduce `ODR` violations too.
  - Integration with MSVC's STL is needed, instead of relying the `nostd` custom implementation (using `abseil` internally).

Why not `CMake`:
  - It's not friendly, neither from the command-line, nor from the IDE.
  - It requires plenty of custom build steps, some from the `ci/` folder, others from `tools/`

  ```
  pwsh ci/setup_windows_ci_environment.ps1
  pwsh ci/do_ci.ps1 cmake.dll.test
  ```
  
To tackle the above issues, this branch creates a dynamic `otel_sdk_r.dll` library, exposing only the OpenTelemetry C++ api:
  - All of the `api` classes are inline, and only few static function members need to be exported, in order to guarantee single stroage for their own static variables ("singletons")
  - Hence `OPENTELEMETRY_API_SINGLETON` would be `__declspec(dllimport)` when using the library.
  - For the `sdk` part (exporters, ext, etc.) the above is not used, but rather `OPENTELEMETRY_EXPORT` (also `dllimport`) for some classes.

## Build settings

- The `.bazelrc` file tries to import `../top.bazelrc`, where you can specify more settings (for example disk cache)
- MSVC Toolset version `14.29.30133` is used (the last from the `v142` series in VS2019). This toolset is also available, as an additional install, in VS2022 too.
- VS2019 was chosen due to: https://learn.microsoft.com/en-us/cpp/porting/binary-compat-2015-2017#restrictions

## How to use

In order to use the library, the users only need to define OPENTELEMETRY_DLL to 1, and include `<opentelemetry/version.h>` as the first file:

```C++
#define OPENTELEMETRY_DLL 1
#include <opentelemetry/version.h> 
#include <opentelemetry/... some other header file>
```

- Some MSVC warnings might have to be disabled too, like: 4505 4251 4275

## Some limitations

Some things are missing:
    - The `protobuf` level of accessing metrics/logs/trace APIs is missing.
    - Some useful plumbing with `grpc` is missing too.

## Building

This branch uses `bazel` to build, and relies on the `//:with_dll` bool flag to configure the build in `dll` mode:

```
bazel test --//:with_dll=true ... -k
```

This produces `otel_sdk_rd.dll` (in the default `fastbuild` configuration, `rd` stands for `release/debug` borrowed from `CMake`'s jargon).

## Packaging

A single `otel_sdk.zip` is produced, mimicking how `vcpkg` exports packages:
    - Single `include/` folder
    - Three `lib` folders: `lib/`, `debug/lib` and `reldeb/lib` (vcpkg only generates the first two)
    - Same for `bin`: `bin`, `debug/bin` and `reldeb/bin`
    - In addition Sentry's `sentrycli` was used to gather (from the .pdb files) all sources used to compile and bundle them in additional .zip files.
