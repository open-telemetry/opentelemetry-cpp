# Jaeger Exporter Example

This is an example of how to use the Jaeger exporter.

The application in `main.cc` initializes an `JaegerExporter` instance and uses it
to register a tracer provider from the [OpenTelemetry
SDK](https://github.com/open-telemetry/opentelemetry-cpp). The application then
calls a `foo_library` which has been instrumented using the [OpenTelemetry
API](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/api).

Resulting spans are exported to the Jaeger agent using the Jaeger exporter.

Note that the Jaeger exporter connects to the agent at `localhost:6831` by
default.

Once you have the Collector running, see
[CONTRIBUTING.md](../../CONTRIBUTING.md) for instructions on building and
running the example.
