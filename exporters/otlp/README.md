# OTLP Exporter

The [OpenTelemetry
Protocol](https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/README.md)
(OTLP) is a vendor-agnostic protocol designed as part of the OpenTelemetry
project. The OTLP exporter can be used to export to any backend that supports
OTLP.

The [OpenTelemetry
Collector](https://github.com/open-telemetry/opentelemetry-collector) is a
reference implementation of an OTLP backend. The Collector can be configured to
export to many other backends, such as Zipkin and Jaegar.

For a full list of backends supported by the Collector, see the [main Collector
repo](https://github.com/open-telemetry/opentelemetry-collector/tree/main/exporter)
and [Collector
contributions](https://github.com/open-telemetry/opentelemetry-collector-contrib/tree/main/exporter).

## Configuration

The OTLP exporter offers some configuration options. To configure the exporter,
create an `OtlpExporterOptions` struct (defined in
[exporter.h](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/exporters/otlp/include/opentelemetry/exporters/otlp/otlp_exporter.h)),
set the options inside, and pass the struct to the `OtlpExporter` constructor,
like so:

```cpp
OtlpExporterOptions options;
options.endpoint = "localhost:12345";
auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpExporter(options));
```

### Configuration options

| Option       | Default          |
| ------------ |----------------- |
| `endpoint`   | `localhost:4317` |

## Example

For a complete example demonstrating how to use the OTLP exporter, see
[examples/otlp](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/examples/otlp/).
