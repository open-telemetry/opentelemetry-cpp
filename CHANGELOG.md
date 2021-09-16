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

## [1.0.0] 2021-09-15

### API

* Document DefaultSpan, remove DefaultTracer ([#959](https://github.com/open-telemetry/opentelemetry-cpp/pull/959))
* Separate baggage<->Context api from Baggage Propagator ([#963](https://github.com/open-telemetry/opentelemetry-cpp/pull/963))
* Remove unused public API to_span_ptr ([#964](https://github.com/open-telemetry/opentelemetry-cpp/pull/964))
* :collision: Make span context management public ([#967](https://github.com/open-telemetry/opentelemetry-cpp/pull/967))
* Support determining parent span from Context while creating new Span ([#969](https://github.com/open-telemetry/opentelemetry-cpp/pull/969))
* :collision: Add note on experimental semantic convention implementation, prefix semantics headers with experimental tag ([#970](https://github.com/open-telemetry/opentelemetry-cpp/pull/970))

### SDK

* Cleanup GetEnvironmentVariable and remove unused variable under NO_GETENV ([#976](https://github.com/open-telemetry/opentelemetry-cpp/pull/976))
* :collision: Add note on experimental semantic convention implementation, prefix semantics headers with experimental tag ([#970](https://github.com/open-telemetry/opentelemetry-cpp/pull/970))

### OTLP Exporter

* :bug: Ignore status description if status code is not Error ([#962](https://github.com/open-telemetry/opentelemetry-cpp/pull/962))
* :collision: Make Otlp exporter configuration environment variables specs-compliant  ([#974](https://github.com/open-telemetry/opentelemetry-cpp/pull/974))

### Zipkin Exporter

* :bug: Don't set parentId in case parentId is empty ([#943](https://github.com/open-telemetry/opentelemetry-cpp/pull/943))
* :rocket: Extend zipkin exporter with ability to provide headers ([#951](https://github.com/open-telemetry/opentelemetry-cpp/pull/951))

### DOCS

* :book: Add Getting started documentation for SDK: ([#942](https://github.com/open-telemetry/opentelemetry-cpp/pull/942))
* :book: Remove unnecessary spaces and spelling of gRPC in README ([#965](https://github.com/open-telemetry/opentelemetry-cpp/pull/965))

### BUILD

* Disable bazel build for gcc 4.8, upgrade versions for grpc(v1.39.1) and bazel(4.2.0), document bazel support ([#953](https://github.com/open-telemetry/opentelemetry-cpp/pull/953))
* Move CMake config template to cmake folder ([#958](https://github.com/open-telemetry/opentelemetry-cpp/pull/958))
* Enable CMake to search the new package variable `<PackageName>_ROOT` ([#975](https://github.com/open-telemetry/opentelemetry-cpp/pull/975))
* :bug: Do not override CMAKE_CXX_STANDARD ([#977](https://github.com/open-telemetry/opentelemetry-cpp/pull/977))

## [1.0.0-rc4] 2021-08-04

* [EXPORTER] `BREAKING CHANGE` Fix default HTTP port for OTLP HTTP Exporter ([#939](https://github.com/open-telemetry/opentelemetry-cpp/pull/939))
* [API] Fix timeout calculation for Tracer::Flush() and Tracer::Close() ([#937](https://github.com/open-telemetry/opentelemetry-cpp/pull/937))
* [API] Fix for Stack::Resize that new_capacity is not assigned to capacity_  ([#934](https://github.com/open-telemetry/opentelemetry-cpp/pull/934))
* [SDK] Fix race condition in circular buffer simulation test ([#931](https://github.com/open-telemetry/opentelemetry-cpp/pull/931))
* [EXPORTER] Fix error logging in Jaeger Exporter ([#930](https://github.com/open-telemetry/opentelemetry-cpp/pull/930))
* [BUILD] Use latest grpc version (v1.39.0) for cmake build of otlp exporter ([#927](https://github.com/open-telemetry/opentelemetry-cpp/pull/927))
* [EXPORTER] Add Jaeger Thrift HTTP exporter ([#926](https://github.com/open-telemetry/opentelemetry-cpp/pull/926))
* [SDK] Move env-var read logic to common dir, and optional reading of env-var ([#924](https://github.com/open-telemetry/opentelemetry-cpp/pull/924))
* [EXPORTER] Remove recordable header from CMake install rules for exporters ([#923](https://github.com/open-telemetry/opentelemetry-cpp/pull/923))
* [EXPORTER] `BREAKING CHANGE` Rename Recordable to JaegerRecordable in Jaeger exporter ([#919](https://github.com/open-telemetry/opentelemetry-cpp/pull/919))
* [EXPORTER] `BREAKING CHANGE` Rename Jaeger exporter target ([#918](https://github.com/open-telemetry/opentelemetry-cpp/pull/918))
* [EXPORTER] Add Zipkin exporter example ([#917](https://github.com/open-telemetry/opentelemetry-cpp/pull/917))
* [EXPORTER] Bazel build for Zipkin exorter ([#916](https://github.com/open-telemetry/opentelemetry-cpp/pull/916))
* [BUILD] Allow to use local GSL ([#915](https://github.com/open-telemetry/opentelemetry-cpp/pull/915))
* [DOCS] Document vcpkg toolchain configuration ([#913](https://github.com/open-telemetry/opentelemetry-cpp/pull/913))
* [SDK] Fix for resource deletion after tracer provider shutdown ([#911](https://github.com/open-telemetry/opentelemetry-cpp/pull/911))
* [BUILD] Add bazel build for grpc example ([#910](https://github.com/open-telemetry/opentelemetry-cpp/pull/910))
* [EXPORTER] Add resource and instrumentation library support for OStreamSpanExporter ([#906](https://github.com/open-telemetry/opentelemetry-cpp/pull/906))
* [API] Adding semantic-convention attributes for trace ([#868](https://github.com/open-telemetry/opentelemetry-cpp/pull/868))

## [1.0.0-rc3] 2021-07-12

* [DOCS] Add doxygen reference docs for SDK ([#902](https://github.com/open-telemetry/opentelemetry-cpp/pull/902))
* [EXPORTER] Jaeger Exporter - Populate resource attributes into process tags in Jaeger ([#897](https://github.com/open-telemetry/opentelemetry-cpp/pull/897))
* [EXPORTER] Zipkin Exporter - Report Event timestamp into microseconds ([#896](https://github.com/open-telemetry/opentelemetry-cpp/pull/896))
* [EXPORTER] Jaeger Exporter - Handle span events ([#895](https://github.com/open-telemetry/opentelemetry-cpp/pull/895))
* [DOCS] Fix API docs for Scope object ([#894](https://github.com/open-telemetry/opentelemetry-cpp/pull/894))
* [BUILD] Fix Linking error for libcurl on some macOS environments ([#893](https://github.com/open-telemetry/opentelemetry-cpp/pull/893))
* [API] Context cleanup from Runtime Storage ([#885](https://github.com/open-telemetry/opentelemetry-cpp/pull/885))
* [DOCS] Document strategy to avoid conflict between two different versions of Abseil ([#883](https://github.com/open-telemetry/opentelemetry-cpp/pull/883/))
* [EXPORTER] ETW Exporter - Document example for ETW Exporter ([#882](https://github.com/open-telemetry/opentelemetry-cpp/pull/882))
* [SDK] Create Span with Valid spanId and traceId irrespective of Sampling decision ([#879](https://github.com/open-telemetry/opentelemetry-cpp/pull/879))
* [EXPORTER] Jaeger Exporter - Rename bswap macros to avoid clash on some systems ([#876](https://github.com/open-telemetry/opentelemetry-cpp/pull/876))
* [API] Add Semantics Conventions attributes for Resources ([#872](https://github.com/open-telemetry/opentelemetry-cpp/pull/872))
* [BUILD] Use nlohmann-json from submodules if not already installed ([#870](https://github.com/open-telemetry/opentelemetry-cpp/pull/870))

## [1.0.0-rc2] 2021-06-18

* [EXPORTER] Jaeger Exporter - Support for Instrumentation Library ([#864](https://github.com/open-telemetry/opentelemetry-cpp/pull/864))
* [TESTS] Adding benchmark tests for baggage api ([#861](https://github.com/open-telemetry/opentelemetry-cpp/pull/861))
* [BUILD] Fix for GCC9/C++20 Support for using STL for modern C++ features ([#860](https://github.com/open-telemetry/opentelemetry-cpp/pull/860))
* [TESTS] Adding benchmark tests for span create api ([#856](https://github.com/open-telemetry/opentelemetry-cpp/pull/856))
* [BUILD] Fix for using Abseil library for modern C++ features ([#850](https://github.com/open-telemetry/opentelemetry-cpp/pull/850))
* [BUILD] Fix issues with win32/x86 compilation ([#847](https://github.com/open-telemetry/opentelemetry-cpp/pull/847))
* [DOCS] Document OSS dependencies and their licenses ([#844](https://github.com/open-telemetry/opentelemetry-cpp/pull/844))
* [BUILD] Various fixes to build with Visual Studio 2015 ([#840](https://github.com/open-telemetry/opentelemetry-cpp/pull/840))
* [INSTRUMENTATION] HTTPClient: Change support for full URL argument ([#833](https://github.com/open-telemetry/opentelemetry-cpp/pull/833))
* [EXPORTER] Jaeger Exporter - fix endianness of Jaeger IDs for transmission ([#832](https://github.com/open-telemetry/opentelemetry-cpp/pull/832))
* [INSTRUMENTATION] fix protobuf compilation warning in gRPC example ([#830](https://github.com/open-telemetry/opentelemetry-cpp/pull/830))
* [EXPORTER] `BREAKING CHANGE` - Add OTLP/HTTP+JSON Protocol exporter; Rename `OtlpExporter` to `OtlpGrpcExporter` ([#810](https://github.com/open-telemetry/opentelemetry-cpp/pull/810))

## [1.0.0-rc1] 2021-06-04

* [BUILD] Enable Jaeger exporter build in Windows ([#815](https://github.com/open-telemetry/opentelemetry-cpp/pull/815))
* [DOCS] Versioning doc update to clarify release and versioning policy ([#811](https://github.com/open-telemetry/opentelemetry-cpp/pull/811))
* [LOGS] Move Logging implementation under feature-flag ([#807](https://github.com/open-telemetry/opentelemetry-cpp/pull/807))
* [BUILD] Filter metric headers files from `opentelemetry-api` and `opentelemetry-sdk` targets if metrics feature-flag is disabled ([#806](https://github.com/open-telemetry/opentelemetry-cpp/pull/806))
* [BUILD] Fix install rule for ostream exporter, Jaeger, ETW, ElasticSearch ([#805](Fix install rule for header files of ostream exporter))
* [API/SDK] Switch from mpark::variant to absl::variant as default ([#771](https://github.com/open-telemetry/opentelemetry-cpp/pull/771))
* [API/SDK] Support `const char *` as acceptable data type for attributes and resources ([#771](https://github.com/open-telemetry/opentelemetry-cpp/pull/771))
* [EXAMPLE] gRPC instrumentation example with context propagation ([#729](https://github.com/open-telemetry/opentelemetry-cpp/pull/729))

## [0.7.0] 2021-05-26

* [METRICS] Move metrics api/sdk under preview feature flag ([#745](https://github.com/open-telemetry/opentelemetry-cpp/pull/745))
* [DOCS] Add instructions to build using Bazel ([#747](https://github.com/open-telemetry/opentelemetry-cpp/pull/747))
* [DOCS] Update copyright headers ([#754](https://github.com/open-telemetry/opentelemetry-cpp/pull/754))
* [EXPORTER] Populate resource to OTLP proto data ([#758](https://github.com/open-telemetry/opentelemetry-cpp/pull/758))
* [CI] Add CodeQL security scan CI workflow ([#770](https://github.com/open-telemetry/opentelemetry-cpp/pull/770))
* [BUILD] Enable building API only CMake Project ([#778](https://github.com/open-telemetry/opentelemetry-cpp/pull/778))
* [SDK] Fix for sampling of root span ([#784](https://github.com/open-telemetry/opentelemetry-cpp/pull/784))
* [CI] Add Jaeger exporter to CMake CI build ([#786](https://github.com/open-telemetry/opentelemetry-cpp/pull/786))
* [API] `BREAKING CHANGE` - Tracer::WithActiveSpan() to return Scope object intead of unique_ptr ([#788](https://github.com/open-telemetry/opentelemetry-cpp/pull/788))
* [DOCS] Add docs for nested spans and context propagation in readthedocs ([#792](https://github.com/open-telemetry/opentelemetry-cpp/pull/792))
* [CI] Output verbose error for failed unit-test in CI ([#796](https://github.com/open-telemetry/opentelemetry-cpp/pull/796))

## [0.6.0] 2021-05-11

* [EXPORTER] Add Jaeger exporter ([#534](https://github.com/open-telemetry/opentelemetry-cpp/pull/534))
* [SDK] Support multiple processors ([#692](https://github.com/open-telemetry/opentelemetry-cpp/pull/692))
* [SDK] Add instrumentation library and multiple tracer support ([#693](https://github.com/open-telemetry/opentelemetry-cpp/pull/693))
* [SDK] Resource integration with Exporters ([#706](https://github.com/open-telemetry/opentelemetry-cpp/pull/706))
* [EXAMPLE] Enhance existing http example with propagation ([#727](https://github.com/open-telemetry/opentelemetry-cpp/pull/727))

## [0.5.0] 2021-04-26

* [SDK] Support custom span-id and trace-id generator ([#681](https://github.com/open-telemetry/opentelemetry-cpp/pull/681))
* [SDK] Add SpanContext (and TraceState) to Recordable ([#667](https://github.com/open-telemetry/opentelemetry-cpp/pull/667))
* [SDK] Global Propagator ([#668](https://github.com/open-telemetry/opentelemetry-cpp/pull/668))
* [SDK] Create SharedContext for updating span pipeline ([#650](https://github.com/open-telemetry/opentelemetry-cpp/pull/650))
* [API] Baggage implementation ([#676](https://github.com/open-telemetry/opentelemetry-cpp/pull/676))
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
