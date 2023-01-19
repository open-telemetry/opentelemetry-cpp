# Windows DLL experiment.

## Before going further, please check and fix .bazelrc

- It's setup to point my own user cache in my F:\ drive, this is probably not what you want (e.g. disk_cache, repository_cache and output_user_root).
- It expects Visual Studio 2019 Build Tools (not Professional, Enterprise or Community) and sets on specific version 14.29.30133 - but you can change all of this.

## This branch is experiment to create useable otel_sdk.dll from the api, sdk and some of the exporters

There are two issues with using OpenTelemetry C++ on Windows:
    - `OPENTELEMETRY_API_SINGLETON` is empty, and singletons are not working (`singleton_test` fails for example), thus making it unuseable if more than one source file uses the library, as each singleton would end up with a copy.
    - The static library version brings a lot of precompiled dependendencies: `grpc`, `protobuf`, etc. 
    - This also makes it possible for more ODR violations, where the client might use different version of these libraries.
    - In fact, this is already an ODR violation - because `grpc` and `jupp0r/prometheus` use different zlib:
        * (Workaround was put in place for this by overriding jupp0r's net_zlib_zlib repo aliasing to grpc's zlib).

To tackle the above issues, this branch creates a dynamic `otel_sdk.dll` library, exposing only the OpenTelemetry C++ api:
    - It declares `OPENTELEMETRY_API_SINGLETON` to be `__declspec(dllimport)` and guarantees the singletons to be in single place (the .dll).
    - There are no other (buildable) dependencies other than the import `otel_sdk.lib` which is small - e.g. it has about thousand .dll import symbols, and no actual code.
    - The above mentioned ODR violations are avoided.

## How to use

In order to use the library, the users would need to define these 3 items, before including opentelemetry headers:
    - `OPENTELEMETRY_DLL=1` - This enables the `dll` path.
    - `HAVE_CPP_STDLIB=1` - The `dll` library was compiled with C++17 and standard STL support (no abseil)
    - `ENABLE_LOGS_PREVIEW=1` - It was also compiled with logs enabled, which is still experimental.

    _(In the future these defines may become part of `version.h` and/or `config.h`)_

```C++
#define OPENTELEMETRY_DLL 1
#define HAVE_CPP_STDLIB 1
#define ENABLE_LOGS_PREVIEW 1
#include <opentelemetry/... some header file>
```

## Some limitations

Not all exporters are included:
    - Jaeger is missing, as it needs more dependencies and these fail to build with bazel.

Some things are missing:
    - The `protobuf` level of accessing metrics/logs/trace APIs is missing.
    - Some useful plumbing with `grpc` is missing too.

## Building

Only the `bazel` build is supported. It needs the `dll` mode to be enabled like this:

```
bazel test --//:with_dll=true ... -k
```

This produces `otel_sdk.dll` (in the default `fastbuild` configuration).

## Packaging

Work in progress, but essentially there would be a target with which one single `otel_sdk.zip` would be created with
all headers, precompiled dlls (for each compiliation mode), and all examples for testing purposes.

It'll mimick how `vcpkg` packages their libs, e.g.:
    - Single `include/` folder
    - Three `lib` folders: `lib/`, `debug/lib` and `reldeb/lib` (vcpkg only generates the first two)
    - Same for `bin`: `bin`, `debug/bin` and `reldeb/bin`

## TODO

There is a `dll_deps` bazel macro, in `dll_deps.bzl` which does something very crude: given the list of dependencies it replaces them with the `//:dll` target, if `//:with_dll=true`
Ideally though it should scan all listed dependencies, and replace only the one that are part of building the `dll`.