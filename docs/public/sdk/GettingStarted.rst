Getting started
^^^^^^^^^^^^^^^

Opentelemetry C++ SDK provides the reference implementation of Opentelemetry C++ API, 
and also provides implementation for Processor, Sampler, and core Exporters as per the 
specification.


Tracing SDK Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

A basic configuration below instantiates the SDK tracer provider and sets to export the traces
to console stream.

Exporter:
^^^^^^^^

An exporter is responsible for sending the telemetry data to a particular backend. 
OpenTelemetry offers six tracing exporters out of the box:
- In-Memory Exporter: keeps the data in memory, useful for debugging.
- Jaeger Exporter: prepares and sends the collected telemetry data to a Jaeger backend via UDP and HTTP.
- Zipkin Exporter: prepares and sends the collected telemetry data to a Zipkin backend via the Zipkin APIs.
- Logging Exporter: saves the telemetry data into log streams
- Opentelemetry(otlp) Exporter: sends the data to the Opentelemetry Collector using protobuf/gRPC or protobuf/HTTP.
- ETW Exporter: sends the telemetry data to Event Tracing for Windows (ETW) 

.. code:: cpp

    // logging exporter
    auto ostream_exporter = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::OStreamSpanExporter);

    // memory exporter
    auto memory_exporter = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::memory::InMemorySpanExporter);

    // zipkin exporter
    opentelemetry::exporter::zipkin::ZipkinExporterOptions opts;
    opts.endpoint = "http://localhost:9411/api/v2/spans" ; // or export OTEL_EXPORTER_ZIPKIN_ENDPOINT="..."
    opts.service_name = "default_service" ; 
    auto zipkin_exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::zipkin::ZipkinExporter(opts));

    // Jaeger UDP exporter
    opentelemetry::exporter::jaeger::JaegerExporterOptions opts;
    opts.server_addr = "localhost";
    opts.server_port =  6831;
    auto jaeger_udp_exporter = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::jaeger::JaegerExporter(opts));

    // otlp grpc exporter
    opentelemetry::exporter::otlp::OtlpGrpcExporterOptions opts;
    opts.endpoint = "localhost::4317";
    opts.use_ssl_credentials = true;
    opts.ssl_credentials_cacert_as_string = "ssl-certificate";
    auto otlp_grpc_exporter = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::otlp::OtlpGrpcExporter(opts));

    // otlp http exporter
    opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;
    opts.url = "http://localhost:4317/v1/traces";
    auto otlp_http_exporter = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::otlp::OtlpHttpExporter(opts));


Span Processor:
^^^^^^^^^^^^^^

Span Processor is initialised with an Exporter. Different Span Processors are offered by Opentelemetry C++ SDK:

- SimpleSpanProcessor: immediately forwards ended spans to the exporter.
- BatchSpanProcessor: batches the ended spans and send them to exporter in bulk.
- MultiSpanProcessor: Allows multiple span processors to be active and configured at the same time.

.. code:: cpp

    // simple processor
    auto simple_processor = std::unique_ptr<sdktrace::SpanProcessor>(
        new sdktrace::SimpleSpanProcessor(std::move(ostream_exporter)));

    // batch processor
    sdktrace::BatchSpanProcessorOptions options{};
    auto batch_processor = std::unique_ptr<sdktrace::SpanProcessor>(
        new sdktrace::BatchSpanProcessor(std::move(memory_exporter), options));
    
    // multi-processor
    std::vector<std::unique_ptr<SpanProcessor>> processors{std::move(simple_processor), std::move(batch_processor)};
    auto multi_processor = std::unique_ptr<sdktrace::SpanProcessor>(
        new sdktrace::MultiSpanProcessor(std::move(processors));

TracerContext
^^^^^^^^^^^^^

SDK configuration shared between `TracerProvider` and all the `Tracer` instances created through it.

.. code:: cpp
    auto exporter = std::unique_ptr<sdktrace::SpanExporter>
        (new opentelemetry::exporter::trace::OStreamSpanExporter);
    auto processor = std::unique_ptr<sdktrace::SpanProcessor>
        (new sdktrace::SimpleSpanProcessor(std::move(exporter)));
    auto 
    auto tracer_context = 