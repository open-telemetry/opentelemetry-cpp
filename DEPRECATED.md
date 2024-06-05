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

### SDK ProviderFactory cleanup

#### Announcement (SDK ProviderFactory cleanup)

* Version: 1.15.0
* Date: 2024-06-03
* PR: [API/SDK] Provider cleanup
  [#2664](https://github.com/open-telemetry/opentelemetry-cpp/pull/2664)

This PR introduces changes to SDK ProviderFactory methods.

#### Motivation (SDK ProviderFactory cleanup)

SDK Factory methods for signal providers, such as:

* opentelemetry::sdk::trace::TracerProviderFactory
* opentelemetry::sdk::metrics::MeterProviderFactory
* opentelemetry::sdk::logs::LoggerProviderFactory
* opentelemetry::sdk::logs::EventLoggerProviderFactory

currently returns a unique pointer on a API class.

This is incorrect, the proper return type should be
a unique pointer on a SDK class instead.

#### Scope (SDK ProviderFactory cleanup)

All the current Create methods in:

* class opentelemetry::sdk::trace::TracerProviderFactory
* class opentelemetry::sdk::metrics::MeterProviderFactory
* class opentelemetry::sdk::logs::LoggerProviderFactory
* class opentelemetry::sdk::logs::EventLoggerProviderFactory

are marked as deprecated, as they return an API object.

Instead, another set of Create methods is provided,
with a different return type, an SDK object.

Both sets can not be exposed at the same time,
as this would cause build breaks,
so a compilation flag is defined to select which methods to use.

When OPENTELEMETRY_DEPRECATED_SDK_FACTORY is defined,
the old, deprecated, methods are available.

When OPENTELEMETRY_DEPRECATED_SDK_FACTORY is not defined,
the new methods are available.

The scope of this deprecation and removal,
is to remove the flag OPENTELEMETRY_DEPRECATED_SDK_FACTORY itself,
which implies that only the new set of Create() methods,
returning an SDK object, are supported.

#### Mitigation (SDK ProviderFactory cleanup)

Build without defining flag OPENTELEMETRY_DEPRECATED_SDK_FACTORY.

Existing code, such as:

```cpp
  std::shared_ptr<opentelemetry::trace::TracerProvider> tracer_provider;
  tracer_provider = opentelemetry::sdk::trace::TracerProviderFactory::Create(...);
```

should be adjusted to:

```cpp
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> tracer_provider;
  tracer_provider = opentelemetry::sdk::trace::TracerProviderFactory::Create(...);
```

#### Planned removal (SDK ProviderFactory cleanup)

Flag OPENTELEMETRY_DEPRECATED_SDK_FACTORY is introduced in release 1.16.0,
to provide a migration path.

This flag is meant to be temporary, and short lived.
Expect removal by release 1.17.0

## [opentelemetry-cpp Exporter]

N/A

## [Documentation]

N/A
