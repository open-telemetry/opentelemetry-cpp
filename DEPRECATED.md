# Deprecated

This document lists all the items currently deprecated in opentelemetry-cpp.

Deprecated items will be removed in the future.

## Guidelines

### Maintainer guidelines

See the [deprecation-process](./docs/deprecation-process.md)

## [TEMPLATE]

### New Deprecation Title (Template)

#### Announcement (Template)

#### Motivation (Template)

#### Scope (Template)

#### Mitigation (Template)

#### Planned removal (Template)

## [Platforms]

N/A

## [Compilers]

N/A

## [Third party dependencies]

N/A

## [Build tools]

N/A

## [Build scripts]

N/A

## [opentelemetry-cpp API]

### Jaeger propagator

#### Announcement (Jaeger)

* Version: 1.8.2
* Date: 2023-01-31
* PR: [DEPRECATION] Deprecate the Jaeger exporter
  [#1923](https://github.com/open-telemetry/opentelemetry-cpp/pull/1923)

This PR also listed the Jaeger propagator as deprecated.

#### Motivation (Jaeger)

The Jaeger Exporter is now (July 2023) removed from the OpenTelemetry specification.

The Jaeger Propagator remains, because changing propagation is a longer
process compared to changing an export format.

New deployments however are encouraged to use a W3C compliant propagator,
and avoid the Jaeger propagator, which is now deprecated.

#### Scope (Jaeger)

The following are deprecated and planned for removal:

* the API header `opentelemetry/trace/propagation/jaeger.h`, including:
  * the C++ class `JaegerPropagator`

#### Mitigation (Jaeger)

Use a W3C compliant propagator instead.

That is, use class HttpTraceContext and "traceparent" tags.

Do not use class JaegerPropagator and "uber-trace-id" tags.

#### Planned removal (Jaeger)

No date set yet for the Jaeger Propagator.

## [opentelemetry-cpp SDK]

N/A

## [opentelemetry-cpp Exporter]

### ZPages exporter

#### Announcement (ZPages)

* Version: 1.11.0
* Date: 2023-09-01
* PR: [DEPRECATION] Deprecate ZPAGES
  [#2291](https://github.com/open-telemetry/opentelemetry-cpp/pull/2291)

#### Motivation (ZPages)

The ZPages specification itself was introduced in 2020,
and has been experimental ever since,
not getting a lot of attention, and never reaching a stable status.

Several other opentelemetry projects have implemented support for zpages,
only to later deprecate and then remove the zpages implementation (Java,
C#), abandoning the zpages feature.

In this context, it does not make sense to continue to maintain the zpages
code in opentelemetry-cpp.

#### Scope (ZPages)

The following are deprecated and planned for removal:

* all the API headers located under
  `ext/include/opentelemetry/ext/zpages/`, including:
  * the C++ class `ThreadsafeSpanData`
  * the C++ class `TracezDataAggregator`
  * the C++ class `TracezHttpServer`
  * the C++ class `TracezSpanProcessor`
  * the C++ class `TracezSharedData`
  * the C++ class `ZPages`
  * the C++ class `zPagesHttpServer`
* all the code and doc located under `ext/src/zpages/`
* all the tests located under `ext/test/zpages/`
* the zpages exporter library(`opentelemetry_zpages`)
* the zpages build options in CMake (`WITH_ZPAGES`)

The following code is no longer considered public, will no longer be
installed, and will no longer be useable outside of
the opentelemetry-cpp implementation:

* all the API headers located under
  `ext/include/opentelemetry/ext/http/server`, including:
  * the C++ class `FileHttpServer`
  * the C++ class `HttpRequestCallback`
  * the C++ class `HttpServer`
  * the C++ class `HttpRequestHandler`
  * the C++ class `SocketCallback`

This implementation of an HTTP server is meant to be used for testing only,
it is not production ready.

#### Mitigation (ZPages)

Consider using a different exporter,
for example the OTLP exporter (both OTLP HTTP and OTLP GRPC are supported),
to expose trace data to a separate trace backend.

Note that this changes the access pattern:

* with zpages, data is only available locally (in process)
* with other exporters, data is available externally (not in process)

Our assessment is that the zpages implementation is no longer in use,
and can be removed.

If that assessment is incorrect (i.e., if you own a project that depends
on the zpage exporter from opentelemetry-cpp), please comment on the
removal issue
[#2292](https://github.com/open-telemetry/opentelemetry-cpp/issues/2292).

An alternative to zpage removal is to move the code to the
opentelemetry-cpp-contrib github
[repository](https://github.com/open-telemetry/opentelemetry-cpp-contrib).

Contributions to migrate the code, and maintain zpages, are welcome.

#### Planned removal (ZPages)

* Date: December, 2023

## [Documentation]

N/A
