# OTLP Exporter Example

This is an example of how to use the [OpenTelemetry
Protocol](https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/README.md)
(OTLP) exporter.

The application in `main.cc` initializes an `OtlpExporter` instance and uses it
to register a tracer provider from the [OpenTelemetry
SDK](https://github.com/open-telemetry/opentelemetry-cpp). The application then
calls a `foo_library` which has been instrumented using the [OpenTelemetry
API](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/api).

Resulting spans are exported to the **OpenTelemetry Collector** using the OTLP
exporter. The OpenTelemetry Collector can be configured to export to other
backends (see list of [supported
backends](https://github.com/open-telemetry/opentelemetry-collector/blob/main/exporter/README.md)).

For instructions on downloading and running the OpenTelemetry Collector, see
[Getting Started](https://opentelemetry.io/docs/collector/about/).

Here is an example of a Collector `config.yaml` file that can be used to export
to [Zipkin](https://zipkin.io/) via the Collector using the OTLP exporter:

```yml
receivers:
  otlp:
    protocols:
      grpc:
        endpoint: localhost:4317
exporters:
  zipkin:
    endpoint: "http://localhost:9411/api/v2/spans"
service:
  pipelines:
    traces:
      receivers: [otlp]
      exporters: [zipkin]
```

Note that the OTLP exporter connects to the Collector at `localhost:4317` by
default. This can be changed with first argument from command-line, for example:
`./example_otlp gateway.docker.internal:4317`.

Once you have the Collector running, see
[CONTRIBUTING.md](../../CONTRIBUTING.md) for instructions on building and
running the example.
