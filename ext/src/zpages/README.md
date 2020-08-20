# zPages
## Overview
zPages are a quick and light way to view tracing and metrics information on standard OpenTelemetry C++ instrumented applications. It requires no external dependencies. See more information in the OTel spec.

# Usage

1. Add the following 2 lines of code
 - `#include opentelemetry/ext/zpages/zpages.h // include zPages`
 - `zpages::Initialize; // start up zPages in your app, before any tracing/span code`
2. Build and run your application normally
 - For example, you can do this for the zPages example while at the root `opentelemetry-cpp` directory with:
   ```
   bazel build //examples/zpages:zpages_example
   bazel-bin/examples/zpages/zpages_example
   ```
   If you look at the zPages example's source [code](https://github.com/open-telemetry/opentelemetry-cpp/blob/master/examples/zpages/zpages_example.cc), it demonstrates adding zPages, manual application instrumentation (which sends data to zPages for viewing), and simulated use cases for zPages.
3. View zPages ib http://localhost:3000/tracez


## More Information
- [zPages General Direction Spec](https://github.com/open-telemetry/oteps/blob/master/text/0110-z-pages.md)
- OTel C++ Design Docs
  - [Tracez Span Processor](https://docs.google.com/document/d/1kO4iZARYyr-EGBlY2VNM3ELU3iw6ZrC58Omup_YT-fU/edit#)
  - [Tracez Data Aggregator](https://docs.google.com/document/d/1ziKFgvhXFfRXZjOlAHQRR-TzcNcTXzg1p2I9oPCEIoU/edit?ts=5ef0d177#heading=h.5irk4csrpu0y)
  - [Tracez Http Server](https://docs.google.com/document/d/1U1V8QZ5LtGl4Mich-aJ6KZGLHrMIE8pWyspmzvnIefI/edit#) - includes pictures of Tracez
