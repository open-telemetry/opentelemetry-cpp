# OTLP Exporter Example

This is an example of how to use the [OpenTelemetry
Protocol](https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/README.md)
(OTLP) exporter.

The application in `grpc_main.cc` initializes an `OtlpExporter` instance and 
the application in `http_main.cc` initializes an `OtlpHttpExporter` instance 
and they register a tracer provider from the [OpenTelemetry
SDK](https://github.com/open-telemetry/opentelemetry-cpp). The application then
calls a `foo_library` which has been instrumented using the [OpenTelemetry
API](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/api).

To enable TLS authentication for OTLP grpc exporter, SslCredentials can be used
by specifying the path to client certificate pem file, or the string containing
this certificate via OtlpExporterOptions. The path to such a .pem file can be
provided as a command-line argument alongwith the collector endpoint to the main
binary invocation above.

Resulting spans are exported to the **OpenTelemetry Collector** using the OTLP
exporter. The OpenTelemetry Collector can be configured to export to other
backends (see list of [supported
backends](https://github.com/open-telemetry/opentelemetry-collector/blob/main/exporter/README.md)).

Follow below command to run the **OpenTelemetry Collector** with OTLP receiver
in docker which dumps the received data into console. See [Getting
Started](https://opentelemetry.io/docs/collector/about/) for more information.

Open a terminal window at the root directory of this repo and launch the
OpenTelemetry Collector with an OTLP receiver by running:

- On Unix based systems use:

```console
docker run --rm -it -p 4317:4317 -v $(pwd)/examples/otlp:/cfg otel/opentelemetry-collector:0.19.0 --config=/cfg/opentelemetry-collector-config/config.dev.yaml
```

- On Windows use:

```console
docker run --rm -it -p 4317:4317 -v "%cd%/examples/otlp":/cfg otel/opentelemetry-collector:0.19.0 --config=/cfg/opentelemetry-collector-config/config.dev.yaml
```

Note that the OTLP exporter connects to the Collector at `localhost:4317` by
default. This can be changed with first argument from command-line, for example:
`./example_otlp_grpc gateway.docker.internal:4317` and 
`./example_otlp_http gateway.docker.internal:4317`..

Once you have the Collector running, see
[CONTRIBUTING.md](../../CONTRIBUTING.md) for instructions on building and
running the example.
