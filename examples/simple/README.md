
# Simple Trace Example

This example sets up a tracer in the main application then makes several calls to a custom library instrumented with the <a href="https://github.com/open-telemetry/opentelemetry-cpp"> Open Telemetry SDK </a>. Specifically, it demonstrates the setup of fundamental Open Telemetry componenets such as Tracers, Tracer Providers and Spans.  The example also illustrates context propogation from parent spans to children spans and events.  All telemetry output is directed to stdout.

## Build and Run
---

<li> Install the latest bazel version by following the steps listed <a href="https://docs.bazel.build/versions/master/install.html">here</a> </li>

<li> Clone or fork the Open Telemetry C++ repository: </li>

```sh
git clone https://github.com/open-telemetry/opentelemetry-cpp.git
```

<li> Build the example from the root of the opentelemetry-cpp directory using Bazel: </li>

```sh
cd opentelemetry-cpp/
bazel build //examples/simple:example_simple
```

<li> Run the resulting executable to see telemetry from the application as it calls the instrumented library: </li>

```sh
bazel-bin/examples/simple/example_simple
```

## Useful Links
---
<li><a href=""> Open Telemetry <a></li>
<li><a href=""> Tracing SDK Specification <a></li>