# Jaeger Exporter for OpenTelemetry C++

## Prerequisite

* [Get Jaeger](https://www.jaegertracing.io/docs/getting-started/) and run
  Jaeger agent.

## Installation

### CMake Installation Instructions

Refer to install instructions
[INSTALL.md](../../INSTALL.md#building-as-standalone-cmake-project). Modify step
2 to create `cmake` build configuration for compiling with Jaeger exporter as
below:

```console
   $ cmake -DWITH_JAEGER=ON ..
   -- The C compiler identification is GNU 9.3.0
   -- The CXX compiler identification is GNU 9.3.0
   ...
   -- Configuring done
   -- Generating done
   -- Build files have been written to: /home/<user>/source/opentelemetry-cpp/build
   $
```

### Bazel install Instructions

Refer to install instructions
[INSTALL.md](../../INSTALL.md#building-as-standalone-bazel-project).

```console
bazel build //exporters/jaeger:opentelemetry_exporter_jaeger_trace
```

## Usage

Install the exporter in your application, initialize and pass the `options` to it.

```cpp
opentelemetry::exporter::jaeger::JaegerExporterOptions options;
options.server_addr      = "localhost";
options.server_port      = 6831;
options.transport_format = opentelemetry::exporter::jaeger::TransportFormat::kThriftUdpCompact;

auto exporter = std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
    new opentelemetry::exporter::jaeger::JaegerExporter(options));
auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
    new sdktrace::SimpleSpanProcessor(std::move(exporter)));
auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
    new sdktrace::TracerProvider(processor));

// Set the global trace provider
opentelemetry::trace::Provider::SetTracerProvider(provider);

```

## Viewing your traces

Please visit the Jaeger UI endpoint <http://localhost:16686>.
