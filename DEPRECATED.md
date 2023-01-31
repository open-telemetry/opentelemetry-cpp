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

### Jaeger

See [Jaeger exporter](#jaeger-exporter)

## [Build tools]

N/A

## [Build scripts]

N/A

## [opentelemetry-cpp API]

N/A

## [opentelemetry-cpp SDK]

N/A

## [opentelemetry-cpp Exporter]

### Jaeger exporter

#### Announcement (Jaeger)

* Version: 1.8.2
* Date: 2023-01-31
* PR: [DEPRECATION] Deprecate the Jaeger exporter
  [#1923](https://github.com/open-telemetry/opentelemetry-cpp/pull/1923)

#### Motivation (Jaeger)

The Jaeger client libraries are deprecated, as announced
[here](https://www.jaegertracing.io/docs/1.41/client-libraries/).

The initial Jaeger announcement in release 1.35 reads as:

"
We plan to continue accepting pull requests and making new releases of
Jaeger clients through the end of 2021. In January 2022 we will enter a code
freeze period for 6 months, during which time we will no longer accept pull
requests with new features, with the exception of security-related fixes.
After that we will archive the client library repositories and will no
longer accept new changes.
"

At time of writing, Jan 2023, the client libraries have been archived 6
months ago already.

#### Scope (Jaeger)

The following are deprecated and planned for removal:

* the API header `opentelemetry/trace/propagation/jaeger.h`, including:
  * the C++ class `JaegerPropagator`
* all the code located under `exporters/jaeger/`, including:
  * the jaeger exporter C++ class (`JaegerExporter`)
  * the related factory (`JaegerExporterFactory`)
  * the related options (`JaegerExporterOptions`)
* the jaeger exporter library(`opentelemetry_exporter_jaeger_trace`)
* the jaeger build options in CMake (`WITH_JAEGER`)
* the dependency on thrift

#### Mitigation (Jaeger)

Jaeger supports natively the OTLP protocol, starting with jaeger 1.35.

An application instrumented with opentelemetry needs to change how the SDK
and exporter are configured to replace the Jaeger exporter with the OTLP
exporter (both OTLP HTTP and OTLP GRPC are supported).

#### Planned removal (Jaeger)

* Date: After April 1st, 2023

## [Documentation]

N/A
