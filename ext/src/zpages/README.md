# zPages

## Overview

zPages are a quick and light way to view tracing and metrics information on
standard OpenTelemetry C++ instrumented applications. It requires no external
dependencies or backend setup. See more information in the OTel zPages
experimental
[spec](https://github.com/open-telemetry/opentelemetry-specification/blob/5b86d4b6c42e6d1e47d9155ac1e2e27f0f0b7769/experimental/trace/zpages.md).
OTel C++ currently only offers Tracez; future zPages to potentially add include
TraceConfigz, RPCz, and Statsz. Events and links need to be added to Tracez.

## Usage

> TODO: Add CMake instructions

1: Add the following 2 lines of code

* `#include opentelemetry/ext/zpages/zpages.h // include zPages`
* `zpages::Initialize; // start up zPages in your app, before any tracing/span
  code`

2: Build and run your application normally

For example, you can do this for the zPages example while at the root
`opentelemetry-cpp` directory with:

```sh
bazel build //examples/zpages:zpages_example
bazel-bin/examples/zpages/zpages_example
```

If you look at the [zPages example's source
code](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/examples/zpages/zpages_example.cc),
it demonstrates adding zPages, manual application instrumentation (which sends
data to zPages for viewing), and simulated use cases for zPages.

3: View zPages at `http://localhost:3000/tracez`

## More Information

* OTel zPages experimental
  [spec](https://github.com/open-telemetry/opentelemetry-specification/blob/5b86d4b6c42e6d1e47d9155ac1e2e27f0f0b7769/experimental/trace/zpages.md)
* [zPages General Direction Spec
  (OTEP)](https://github.com/open-telemetry/oteps/blob/main/text/0110-z-pages.md)
* OTel C++ Design Docs
  * [Tracez Span
    Processor](https://docs.google.com/document/d/1kO4iZARYyr-EGBlY2VNM3ELU3iw6ZrC58Omup_YT-fU/edit#)
  * [Tracez Data
    Aggregator](https://docs.google.com/document/d/1ziKFgvhXFfRXZjOlAHQRR-TzcNcTXzg1p2I9oPCEIoU/edit?ts=5ef0d177#heading=h.5irk4csrpu0y)
  * [Tracez Http
    Server](https://docs.google.com/document/d/1U1V8QZ5LtGl4Mich-aJ6KZGLHrMIE8pWyspmzvnIefI/edit#)
    * includes reference pictures of the zPages/Tracez UI
