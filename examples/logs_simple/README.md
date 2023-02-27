
# Simple Logs Example

In this example, the application in `main.cc` initializes an
`OStreamLogRecordExporter` instance and registers a logger
provider, as well as initializes a `StdoutSpanExporter` instance and registers a
tracer provider from the [OpenTelemetry
SDK](https://github.com/open-telemetry/opentelemetry-cpp).

The application then calls a `logs_foo_library` which has been instrumented
using the [OpenTelemetry
API](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/api).
Resulting logs and traces are directed to stdout.

See [CONTRIBUTING.md](../../CONTRIBUTING.md) for instructions on building and
running the example.
