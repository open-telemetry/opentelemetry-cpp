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

### Drop C++11 support

#### Announcement (C++11)

* Date: 2022-12-01
* Issue: [DEPRECATION] Drop C++11 support
  [#1830](https://github.com/open-telemetry/opentelemetry-cpp/pull/1830)
* This announcement has been pinned,
  visible in the issues pages since December 2022.

#### Motivation (C++11)

This repository, opentelemetry-cpp, supports a "bring your own dependency" model.

In this model,
the build scripts can be configured to:

* pick a given version for a third party library,
* build opentelemetry-cpp with the library given.

The makefiles do not mandate to use a particular version,
hence the "bring your own" denomination.

To have an up to date build, projects are encouraged to use up to date
versions of third party libraries, to benefit from bug fixes.

Now, many libraries deliver new versions that require C++14, bug fixes
releases for C++11 are no longer available.

In particular, the following components:

* GRPC C++
* abseil
* googletest

now require C++14, per
[google support policies](https://github.com/google/oss-policies-info/blob/main/foundational-cxx-support-matrix.md)

As a result, to stay up to date, opentelemetry-cpp needs to upgrade its
minimum build requirements to use C++14 instead of C++11.

#### Scope (C++11)

Continuous Integration (CI) builds will use C++14 instead of C++11.

The CI build for gcc 4.8 is now deprecated, to be decommissioned when C++11
support is dropped.

#### Mitigation (C++11)

Building the code with recent third party libraries will require C++14
instead of C++11.

#### Planned end of life (C++11)

Support for C++11 in opentelemetry-cpp will end on September 2023.

After this date, opentelemetry-cpp may still build properly in C++11 mode,
assuming a suitable, old, version for each dependency is used,
but the C++11 build will no longer be tested for each new release.

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
