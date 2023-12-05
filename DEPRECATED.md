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

N/A

## [Documentation]

N/A
