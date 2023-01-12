# Deprecated

This document lists all the items currently deprecated in opentelemetry-cpp.

Deprecated items will be removed in the future.

## Guidelines

### Maintainer guidelines

### User guidelines

## [Platforms]

N/A

## [Compilers]

N/A

## [Third party dependencies]

### Jaeger

See [Jaeger exporter](./Jaeger exporter)

## [Build tools]

N/A

## [Build scripts]

N/A

## [OpenTelemetry-cpp API]

N/A

## [OpenTelemetry-cpp SDK]

N/A

## [OpenTelemetry-cpp Exporter]

### Jaeger exporter

#### Announcement

* Version: TO-BE-RELEASED-VERSION
* Date: TO-BE-RELEASED-DATE
* PR: [DEPRECATION] Deprecate the Jaeger exporter
  [#9999](https://github.com/open-telemetry/opentelemetry-cpp/pull/9999)

#### Motivation

The jaeger client libraries are deprecated, as announced
[here](https://www.jaegertracing.io/docs/1.41/client-libraries/).

The initial jaeger announcement in release 1.35 reads as:

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

#### Scope

The following are deprecated and planned for removal:

- all the code located under `exporters/jaeger/`, including:
  - the jaeger exporter C++ class (`JaegerExporter`)
  - the related factory (`JaegerExporterFactory`)
  - the related options (`JaegerExporterOptions`)
- the jaeger exporter library(`opentelemetry_exporter_jaeger_trace`)
- the jaeger build options in CMake (`WITH_JAEGER`)
- the dependency on thrift

#### Mitigation

Jaeger supports natively the OTLP protocol, starting with jaeger 1.35.

An application instrumented with opentelemetry needs to change how the SDK
and exporter are configured to replace the Jaeger exporter with the OTLP
exporter (both OTLP HTTP and OTLP GRPC are supported).


#### Planned removal

* Date: After April 1st, 2023

## [Documentation]

N/A

