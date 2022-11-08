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
create an `OtlpGrpcExporterOptions` struct (defined in
[otlp_grpc_exporter.h](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/exporters/otlp/include/opentelemetry/exporters/otlp/otlp_grpc_exporter.h)),
set the options inside, and pass the struct to the `OtlpGrpcExporter` constructor,
like so:

```cpp
OtlpGrpcExporterOptions options;
options.endpoint = "localhost:12345";
auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpGrpcExporter(options));
```

The OTLP HTTP exporter offers some configuration options. To configure the exporter,
create an `OtlpHttpExporterOptions` struct (defined in
[otlp_http_exporter.h](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/exporters/otlp/include/opentelemetry/exporters/otlp/otlp_http_exporter.h)),
set the options inside, and pass the struct to the `OtlpHttpExporter` constructor,
like so:

```cpp
OtlpHttpExporterOptions options;
options.url = "localhost:12345";
auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpHttpExporter(options));
```

### Configuration options ( OTLP GRPC Exporter )

| Option                           | Env Variable                                 | Default               | Description                          |
|----------------------------------|----------------------------------------------|-----------------------|--------------------------------------|
|`endpoint`                        |`OTEL_EXPORTER_OTLP_ENDPOINT`                 |`http://localhost:4317`| The OTLP GRPC endpoint to connect to |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_ENDPOINT`          |                       |                                      |
|`use_ssl_credentials`             |`OTEL_EXPORTER_OTLP_SSL_ENABLE`               | `false`               | Whether the endpoint is SSL enabled  |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_SSL_ENABLE`        |                       |                                      |
|`ssl_credentials_cacert_path`     |`OTEL_EXPORTER_OTLP_CERTIFICATE`              | `""`                  | SSL Certificate file path            |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE`       |                       |                                      |
|`ssl_credentials_cacert_as_string`|`OTEL_EXPORTER_OTLP_CERTIFICATE_STRING`       | `""`                  | SSL Certifcate as in-memory string   |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_CERTIFICATE_STRING`|                       |                                      |
|`timeout`                         |`OTEL_EXPORTER_OTLP_TIMEOUT`                  | `10s`                 | GRPC deadline                        |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_TIMEOUT`           |                       |                                      |
|`metadata`                        |`OTEL_EXPORTER_OTLP_HEADERS`                  |                       | Custom metadata for GRPC             |
|                                  |`OTEL_EXPORTER_OTLP_TRACES_HEADERS`           |                       |                                      |

### Configuration options ( OTLP HTTP Exporter )

| Option             | Env Variable                       | Default                         | Description                                                       |
|--------------------|------------------------------------|---------------------------------|-------------------------------------------------------------------|
|`url`               |`OTEL_EXPORTER_OTLP_ENDPOINT`       |`http://localhost:4318/v1/traces`| The OTLP HTTP endpoint to connect to                              |
|                    |`OTEL_EXPORTER_OTLP_TRACES_ENDPOINT`|                                 |                                                                   |
|`content_type`      | n/a                                | `application/json`              | Data format used - JSON or Binary                                 |
|`json_bytes_mapping`| n/a                                | `JsonBytesMappingKind::kHexId`  | Encoding used for trace_id and span_id                            |
|`use_json_name`     | n/a                                | `false`                         | Whether to use json name of protobuf field to set the key of json |
|`timeout`           |`OTEL_EXPORTER_OTLP_TIMEOUT`        | `10s`                           | http timeout                                                      |
|                    |`OTEL_EXPORTER_OTLP_TRACES_TIMEOUT` |                                 |                                                                   |
|`http_headers`      |`OTEL_EXPORTER_OTLP_HEADERS`        |                                 | http headers                                                      |
|                    |`OTEL_EXPORTER_OTLP_TRACES_HEADERS` |                                 |                                                                   |

## Example

For a complete example demonstrating how to use the OTLP exporter, see
[examples/otlp](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/examples/otlp/).
