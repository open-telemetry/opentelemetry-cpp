# OpenTracing Shim

[![Apache License][license-image]][license-image]

The OpenTracing shim is a bridge layer from OpenTelemetry to the OpenTracing API.
It takes an OpenTelemetry Tracer and exposes it as an implementation compatible with
that of an OpenTracing Tracer.

## Usage

Use the TracerShim wherever you initialize your OpenTracing tracers.
There are 2 ways to expose an OpenTracing Tracer:

1. From the global OpenTelemetry configuration:

    ```cpp
    auto tracer_shim = TracerShim::createTracerShim();
    ```

1. From a provided OpenTelemetry Tracer instance:

    ```cpp
    auto tracer_shim = TracerShim::createTracerShim(tracer);
    ```

Optionally, one can also specify the propagators to be used for the OpenTracing `TextMap`
and `HttpHeaders` formats:

```cpp
OpenTracingPropagators propagators{
  .text_map = nostd::shared_ptr<TextMapPropagator>(new CustomTextMap()),
  .http_headers = nostd::shared_ptr<TextMapPropagator>(new trace::propagation::HttpTraceContext())
};

auto tracer_shim = TracerShim::createTracerShim(tracer, propagators);
```

If propagators are not specified, OpenTelemetry's global propagator will be used.

## License

Apache 2.0 - See [LICENSE][license-url] for more information.

## Useful links

- For more information on OpenTelemetry, visit: <https://opentelemetry.io/>
- For help or feedback on this project, join us in [GitHub Discussions][discussions-url]

[discussions-url]: https://github.com/open-telemetry/opentelemetry-cpp/discussions
[license-url]: https://github.com/open-telemetry/opentelemetry-cpp/blob/main/LICENSE
[license-image]: https://img.shields.io/badge/license-Apache_2.0-green.svg?style=flat
