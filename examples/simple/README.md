
# Simple Trace Example

In this example, an application in `main.cc` initializes and registers a tracer provider from the <a href="https://github.com/open-telemetry/opentelemetry-cpp"> Open Telemetry SDK </a>.  The application then calls a `foo_library` which has been manually instrumented using the <a href="https://github.com/open-telemetry/opentelemetry-cpp/tree/master/api"> OpenTelemetry API </a>.  Resulting telemetry is directed to stdout by through a custom exporter.

See <a href="https://github.com/open-telemetry/opentelemetry-cpp/blob/master/CONTRIBUTING.md"> CONTRIBUTING.md </a> for instructions on building and running the example.