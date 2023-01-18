# Windows DLL experiment.

## This branch is experiment to create useable otel_sdk.dll from the api, sdk and some of the exporters

There are two issues with using OpenTelemetry C++ on Windows:
    - OPENTELEMETRY_API_SINGLETON is not working (singleton_test fails for example), thus making it unuseable if more than one source file uses it.
    - The static library version brings a lot of precompiled dependendencies, grpc, protobuf, etc. Also it's possible to make ODR violations if the user app uses different version of some of these libraries.

Instead, this experiment tries to create single otel_sdk.dll exporting API, SDK, Exporters and Extensions.

To simplify the use case, C++17 is assumed, and standard STL libraries are preferred over custom ones - e.g. HAVE_CPP_STDLIB=1

## How to use

The final users, would only need the opentelemetry headers, the import otel_sdk.lib, and to define OPENTELEMETRY_DLL=1 (and HAVE_CPP_STDLIB=1 for now).
The library then would define OPENTELEMETRY_API=__declspec(dllimport)

## Some limitations

Not all exporters are included:
    - Jaeger is missing, as it needs more dependencies to build.
    - Prometheus has clash with zlib right now (ODR), hence /WHOLEARCHIVE does not work.
    - Zipkin works, but nlohmann/json is not included so some of the examples/tests using it may not work.
    - No ETW, and Elasticsearch yet, but these can be added later.

## Building

Only bazel build is supported, and the `dll` version is enabled by turning on custom flag:

bazel test --//:with_dll=true ... -k
