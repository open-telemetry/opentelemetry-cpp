# Overview

The OpenTelemetry C++ API enables developers to instrument their applications
and libraries in order to make them ready to create and emit telemetry data.
The OpenTelemetry C++ API exclusively focuses on instrumentation and does not
address concerns like exporting, sampling, and aggregating telemetry data.
Those concerns are addressed by the OpenTelemetry C++ SDK. This architecture
enables developers to instrument applications and libraries with the
OpenTelemetry C++ API while being completely agnostic of how telemetry data is
exported and processed.

## Library design

The OpenTelemetry C++ API is provided as a header-only library and supports all
recent versions of the C++ standard, down to C++11.

A single application might dynamically or statically link to different libraries
that were compiled with different compilers, while several of the linked
libraries are instrumented with OpenTelemetry. OpenTelemetry C++ supports those
scenarios by providing a stable ABI. This is achieved by a careful API design,
and most notably by providing ABI stable versions of classes from the standard
library. All those classes are provided in the `opentelemetry::nostd` namespace.

## Getting started

### Tracing

When instrumenting libraries and applications, the most simple approach
requires three steps.

#### Obtain a tracer

```cpp
auto provider = opentelemetry::trace::Provider::GetTracerProvider();
auto tracer = provider->GetTracer("foo_library", "1.0.0");
```

The `TracerProvider` acquired in the first step is a singleton object that
is usually provided by the OpenTelemetry C++ SDK. It is used to provide
specific implementations for API interfaces. In case no SDK is used, the API
provides a default no-op implementation of a `TracerProvider`.

The `Tracer` acquired in the second step is needed to create and start `Span`s.

#### Start a span

```cpp
auto span = tracer->StartSpan("HandleRequest");
```

This creates a span, sets its name to `"HandleRequest"`, and sets its start
time to the current time. Refer to the API documentation for other operations
that are available to enrich spans with additional data.

#### Mark a span as active

```cpp
auto scope = tracer->WithActiveSpan(span);
```

This marks a span as active and returns a `Scope` object bound to the
lifetime of the span. When the `Scope` object is destroyed, the related span is
ended.

The concept of an active span is important, as any span that is created
without explicitly specifying a parent is parented to the currently active
span.

## Getting help

* Refer to [opentelemetry.io](https://opentelemetry.io/) for general
  information about OpenTelemetry.
* Refer to the [OpenTelemetry C++ GitHub repository](https://github.com/open-telemetry/opentelemetry-cpp)
  for further information and resources related to OpenTelemetry C++.
* For questions related to OpenTelemetry C++ that are not covered by the
  existing documentation, please ask away in [GitHub discussions](https://github.com/open-telemetry/opentelemetry-cpp/discussions).
* Feel free to join the [CNCF OpenTelemetry C++ Slack channel](https://cloud-native.slack.com/archives/C01N3AT62SJ).
  If you are new, you can create a CNCF Slack account [here](http://slack.cncf.io/).
* For bugs and feature requests, write a [GitHub issue](https://github.com/open-telemetry/opentelemetry-cpp/issues).
