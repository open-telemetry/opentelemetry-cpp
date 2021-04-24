# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Guideline to update the version

Increment the:

* MAJOR version when you make incompatible API/ABI changes,
* MINOR version when you add functionality in a backwards compatible manner, and
* PATCH version when you make backwards compatible bug fixes.

## [Unreleased]

* [SDK] Add instrumentation library and multiple tracer support ([#693](https://github.com/open-telemetry/opentelemetry-cpp/pull/693))
* [API] Move class from opentelemetry::core namespace to opentelemetry::common namespace ([#686](https://github.com/open-telemetry/opentelemetry-cpp/pull/686))

## [0.4.0] 2021-04-12

* [EXPORTER] ETW Exporter enhancements ([#519](https://github.com/open-telemetry/opentelemetry-cpp/pull/519))
* [EXPORTER] Read Zipkin endpoint from environment variable. ([#624](https://github.com/open-telemetry/opentelemetry-cpp/pull/624))
* [EXPORTER] Split Zpages webserver hosting from Exporter ([#626](https://github.com/open-telemetry/opentelemetry-cpp/pull/626))
* [EXPORTER] ETW Exporter Usage Instructions ([#628](https://github.com/open-telemetry/opentelemetry-cpp/pull/628))
* [INSTRUMENTATION] HTTP Client/Server Instrumentation example ([#632](https://github.com/open-telemetry/opentelemetry-cpp/pull/632))
* [EXPORTER] Enable tls authentication for otlp grpc exporter ([#635](Enable tls authentication for otlp grpc exporter))
* [API] Refactoring trace_state to reuse common functionality in baggage ([#638](https://github.com/open-telemetry/opentelemetry-cpp/pull/638/files))

## [0.3.0] 2021-03-19

* [EXPORTER] Added Zipkin Exporter. ([#471](https://github.com/open-telemetry/opentelemetry-cpp/pull/471))
* [API] Added Jaeger propagator. ([#599](https://github.com/open-telemetry/opentelemetry-cpp/pull/599))
* [PROPAGATOR] Added Composite Propagator ([#597](https://github.com/open-telemetry/opentelemetry-cpp/pull/597))
* [API] Propagate traceflag from parent ([#603](https://github.com/open-telemetry/opentelemetry-cpp/pull/603))
* [DOCS] Add sphinx support for api doc generation ([595](https://github.com/open-telemetry/opentelemetry-cpp/pull/595))
* [SDK] Add service.name if missing in Resource ([#616](https://github.com/open-telemetry/opentelemetry-cpp/pull/616))

## [0.2.0] 2021-03-02

* [SDK] Added `ForceFlush` to `TracerProvider`. ([#588](https://github.com/open-telemetry/opentelemetry-cpp/pull/588)).
* [SDK] Added Resource API.  ([#502](https://github.com/open-telemetry/opentelemetry-cpp/pull/502))
* [API] Modified TraceState support for w3c trace context as per specs.
([#551](https://github.com/open-telemetry/opentelemetry-cpp/pull/551))
* [API] Added B3 Propagator. ([#523](https://github.com/open-telemetry/opentelemetry-cpp/pull/523))
* [Exporter] Added ETW Exporter. ([#376](https://github.com/open-telemetry/opentelemetry-cpp/pull/376))
* [CI] Enable cache for Bazel for faster builds. ([#505](https://github.com/open-telemetry/opentelemetry-cpp/pull/505))

## [0.0.1] 2020-12-16

### Added

* Trace API and SDK experimental
* OTLP Exporter

### Changed

### Removed
