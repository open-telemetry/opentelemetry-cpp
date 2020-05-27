
## Building and Running the Simple Trace Example

This example sets up a tracer in the main application then uses a library that is instrumented with Open Telemetry. All telemetry output is directed to stdout.  

Install Bazel version 1.2.0 by following the steps listed at https://docs.bazel.build/versions/master/install.html

Clone or fork the Open Telemetry C++ repository:

```sh
git clone https://github.com/open-telemetry/opentelemetry-cpp.git
```

Build the example from the root of the opentelemetry-cpp directory using Bazel:

```sh
cd opentelemetry-cpp/
bazel build //examples/simple:example_simple
```

Run the resulting executable to see telemetry from the application as it calls the instrumented library.  

```sh
bazel-bin/examples/simple/example_simple
```