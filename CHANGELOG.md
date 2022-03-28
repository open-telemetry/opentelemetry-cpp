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

* [SDK] Bugfix: span SetAttribute crash ([#1283](https://github.com/open-telemetry/opentelemetry-cpp/pull/1283))
* [EXPORTER] Jaeger Exporter - Populate Span Links ([#1251](https://github.com/open-telemetry/opentelemetry-cpp/pull/1251))

## [1.2.0] 2022-01-31

* [CI] Continuous benchmark tests as part of the CI ([#1174](https://github.com/open-telemetry/opentelemetry-cpp/pull/1174))
* [API] Allow to use external abseil for bazel targets ([#1172](https://github.com/open-telemetry/opentelemetry-cpp/pull/1172))
* [EXPORTER] Importing gsl::span if std::span is not available ([#1167](https://github.com/open-telemetry/opentelemetry-cpp/pull/1167))
* [EXPORTER] Synchronized calls to Exporter::Export & Shutdown ([#1164](https://github.com/open-telemetry/opentelemetry-cpp/pull/1164))
* [EXPORTER] OTLP http exporter block thread  ([#1163](https://github.com/open-telemetry/opentelemetry-cpp/pull/1163))
* [TESTS] Jaeger: ThriftSender unit test ([#1162](https://github.com/open-telemetry/opentelemetry-cpp/pull/1162))
* [EXPORTER] InMemorySpanExporter shutdown fix ([#1161](https://github.com/open-telemetry/opentelemetry-cpp/pull/1161))
* [EXPORTER] Fix leak in Jaeger exporter ([#1160](https://github.com/open-telemetry/opentelemetry-cpp/pull/1160))
* [TESTS] ZipkinExporter unit-tests ([#1155](https://github.com/open-telemetry/opentelemetry-cpp/pull/1155))
* [SDK] Logger: propagating resources through LoggerProvider ([#1154](https://github.com/open-telemetry/opentelemetry-cpp/pull/1154))
* [SDK] Logger: support for instrumentation library ([#1149](https://github.com/open-telemetry/opentelemetry-cpp/pull/1149))
* [SDK] Add log level for internal log of sdk ([#1147](https://github.com/open-telemetry/opentelemetry-cpp/pull/1147))
* [METRICS] Metrics SDK: View API ([#1110](https://github.com/open-telemetry/opentelemetry-cpp/pull/1110))

Notes on experimental features:

[#1149](https://github.com/open-telemetry/opentelemetry-cpp/pull/1149) and
[#1154](https://github.com/open-telemetry/opentelemetry-cpp/pull/1154) from
above CHANGELOG introduced API changes which are not backward compatible with
previous logs, please update API package to this release if
`ENABLE_LOGS_PREVIEW` is turned on (it is turned off by default).

## [1.1.1] 2021-12-20

* [SDK] Rename OTEL_CPP_GET_ATTR macro, and define it using fully qualified attr
  function
  ([#1140](https://github.com/open-telemetry/opentelemetry-cpp/pull/1140))
* [SDK] Default resource attributes and attributes in OTEL_RESOURCE_ATTRIBUTES
  are missing when using Otlp*LogExporter
  ([#1082](https://github.com/open-telemetry/opentelemetry-cpp/pull/1082))
* [METRICS] Add Meter and MeterProvider in the SDK
  ([#1078](https://github.com/open-telemetry/opentelemetry-cpp/pull/1078))
* [EXPORTER] ZipkinExporter shutdown
  ([#1153](https://github.com/open-telemetry/opentelemetry-cpp/pull/1153))
* [EXPORTER] Jaeger exporter shutdown
  ([#1150](https://github.com/open-telemetry/opentelemetry-cpp/pull/1150))
* [EXPORTER] Bugfix: `jaeger::TUDPTransport::write` crash when `getaddrinfo`
  returns error
  ([#1116](https://github.com/open-telemetry/opentelemetry-cpp/pull/1116))
* [EXPORTER] Bugfix: Jaeger exporter: extend supported attributes types
  ([#1106](https://github.com/open-telemetry/opentelemetry-cpp/pull/1106))
* [EXPORTER] Fix otlp generates null span ids
  ([#1113](https://github.com/open-telemetry/opentelemetry-cpp/pull/1113))
* [EXPORTER] Jaeger bazel (Linux only)
  ([#1077](https://github.com/open-telemetry/opentelemetry-cpp/pull/1077))
* [DOCS] Add note on DLL support
  ([#1137](https://github.com/open-telemetry/opentelemetry-cpp/pull/1137))
* [DOCS] Improve the instructions for Bazel build
  ([#1136](https://github.com/open-telemetry/opentelemetry-cpp/pull/1136))
* [DOCS] Document dependencies
  ([#1119](https://github.com/open-telemetry/opentelemetry-cpp/pull/1119))
* [DOCS] Dockerfile for quick demo/troubleshooting purpose
  ([#905](https://github.com/open-telemetry/opentelemetry-cpp/pull/905))
* [TESTS] Fix data race in BM_ThreadYieldSpinLockThrashing
  ([#1099](https://github.com/open-telemetry/opentelemetry-cpp/pull/1099))
* [EXAMPLE] Otlp gRPC log example
  ([#1083](https://github.com/open-telemetry/opentelemetry-cpp/pull/1083))
* [BUILD] C++20 not Building with VS2019
  ([#1144](https://github.com/open-telemetry/opentelemetry-cpp/pull/1144))
* [BUILD] Mark tags to bazel
  targets([#1075](https://github.com/open-telemetry/opentelemetry-cpp/pull/1075))

## [1.1.0] 2021-11-19

* [BUILD] build release tarball when nlohmann-json not installed
  ([#1074](https://github.com/open-telemetry/opentelemetry-cpp/pull/1074))
* [SDK] Bugfix: regex is neither working on GCC 4.9.x
  ([#1069](https://github.com/open-telemetry/opentelemetry-cpp/pull/1069))
* [SDK] Improvement: span_id should not break strict aliasing.
  ([#1068](https://github.com/open-telemetry/opentelemetry-cpp/pull/1068))
* [EXAMPLE] OTLP HTTP log example
  ([#1062](https://github.com/open-telemetry/opentelemetry-cpp/pull/1062))
* [SDK] OTLP gRPC log export should fail after shutdown
  ([#1064](https://github.com/open-telemetry/opentelemetry-cpp/pull/1064))
* [BUILD] Building otlp exporter from the release tarball
  ([#1056](https://github.com/open-telemetry/opentelemetry-cpp/pull/1056))
* [METRICS] Move old metrics implementation to different directory, and rename
  targets to \_deprecated
  ([#1053](https://github.com/open-telemetry/opentelemetry-cpp/pull/1053))
* [EXPORTER] Add OTLP/gRPC Log Exporter
  ([#1048](https://github.com/open-telemetry/opentelemetry-cpp/pull/1048))
* [EXPORTER] Prometheus Exporter
  ([#1031](https://github.com/open-telemetry/opentelemetry-cpp/pull/1031))
* [EXPORTER] Add OTLP/HTTP Log Exporter
  ([#1030](https://github.com/open-telemetry/opentelemetry-cpp/pull/1030))
* [SDK] fix: issue 368- consistent namespace scope resolution
  ([#1008](https://github.com/open-telemetry/opentelemetry-cpp/pull/1008))

## [1.0.1] 2021-10-21

* [EXPORTER] Exports span attributes to ETW
  ([#1021](https://github.com/open-telemetry/opentelemetry-cpp/pull/1021))
* [BUILD] cmake: add FindThrift.cmake find module method for thrift
  ([#1020](https://github.com/open-telemetry/opentelemetry-cpp/pull/1020))
* [BUILD] Fix nlohmann_json package dependency
  ([#1017](https://github.com/open-telemetry/opentelemetry-cpp/pull/1017))
* [EXPORTER] Change OTLP/HTTP default port from 4317 to 4318
  ([#1018](https://github.com/open-telemetry/opentelemetry-cpp/pull/1018))
* [EXPORTER] ETW Log Exporter
  ([#1006](https://github.com/open-telemetry/opentelemetry-cpp/pull/1006))
* [API] Adding new Logger:log() method
  ([#1005](https://github.com/open-telemetry/opentelemetry-cpp/pull/1005))
* [EXPORTER] Remove scheme from OTLP endpoint before passing to gRPC
  ([#988](https://github.com/open-telemetry/opentelemetry-cpp/pull/988))
* [BUILD] Update opentelemetry-proto for bazel build to 0.9.0
  ([#984](https://github.com/open-telemetry/opentelemetry-cpp/pull/984))
* [BUILD] Cross compling grpc_cpp_plugin not found bug
  ([#982](https://github.com/open-telemetry/opentelemetry-cpp/pull/982))
* [EXPORTER] Support environment variables for both `OtlpGrpcExporter` and
  `OtlpHttpExporter`
  ([#983](https://github.com/open-telemetry/opentelemetry-cpp/pull/983))
* [API/SDK] Add schema_url support to both Resource and InstrumentationLibrary
  ([#979](https://github.com/open-telemetry/opentelemetry-cpp/pull/979))
* [BUILD] Fix build issue where _memcpy_ was not declared in scope
  ([#985](https://github.com/open-telemetry/opentelemetry-cpp/issues/985))

## [1.0.0] 2021-09-16

### API

* Document DefaultSpan, remove DefaultTracer
  ([#959](https://github.com/open-telemetry/opentelemetry-cpp/pull/959))
* Separate baggage<->Context api from Baggage Propagator
  ([#963](https://github.com/open-telemetry/opentelemetry-cpp/pull/963))
* Remove unused public API to_span_ptr
  ([#964](https://github.com/open-telemetry/opentelemetry-cpp/pull/964))
* :collision: Make span context management public
  ([#967](https://github.com/open-telemetry/opentelemetry-cpp/pull/967))
* Support determining parent span from Context while creating new Span
  ([#969](https://github.com/open-telemetry/opentelemetry-cpp/pull/969))
* :collision: Traces: Add note on experimental semantic convention
  implementation, prefix semantics headers with experimental tag
  ([#970](https://github.com/open-telemetry/opentelemetry-cpp/pull/970))
* Increment OPENTELEMETRY_ABI_VERSION_NO to 1
  ([#980](https://github.com/open-telemetry/opentelemetry-cpp/pull/980))

### SDK

* Clean up `GetEnvironmentVariable` and remove unused variable under `NO_GETENV`
  ([#976](https://github.com/open-telemetry/opentelemetry-cpp/pull/976))
* :collision: Resources: Add note on experimental semantic convention
  implementation, prefix semantics headers with experimental tag
  ([#970](https://github.com/open-telemetry/opentelemetry-cpp/pull/970))

### OTLP Exporter

* :bug: Ignore status description if status code is not Error
  ([#962](https://github.com/open-telemetry/opentelemetry-cpp/pull/962))
* :collision: Make Otlp exporter configuration environment variables
  specs-compliant
  ([#974](https://github.com/open-telemetry/opentelemetry-cpp/pull/974))

### Zipkin Exporter

* :bug: Don't set parentId in case parentId is empty
  ([#943](https://github.com/open-telemetry/opentelemetry-cpp/pull/943))
* :rocket: Extend zipkin exporter with ability to provide headers
  ([#951](https://github.com/open-telemetry/opentelemetry-cpp/pull/951))

### DOCS

* :book: Add getting-started documentation for SDK:
  ([#942](https://github.com/open-telemetry/opentelemetry-cpp/pull/942))
* :book: Remove unnecessary spaces and spelling of gRPC in README
  ([#965](https://github.com/open-telemetry/opentelemetry-cpp/pull/965))

### BUILD

* Disable bazel build for gcc 4.8, upgrade versions for grpc(v1.39.1) and
  bazel(4.2.0), document bazel support
  ([#953](https://github.com/open-telemetry/opentelemetry-cpp/pull/953))
* Move CMake config template to cmake folder
  ([#958](https://github.com/open-telemetry/opentelemetry-cpp/pull/958))
* Enable CMake to search the new package variable `<PackageName>_ROOT`
  ([#975](https://github.com/open-telemetry/opentelemetry-cpp/pull/975))
* :bug: Do not override CMAKE_CXX_STANDARD
  ([#977](https://github.com/open-telemetry/opentelemetry-cpp/pull/977))

### :construction: Experiemental Features (Will change in future)

* Semantic Conventions for traces - As the specs is still experimental, the
  implementation will change in future.
* Semantic Convention for resource - As the specs is still experimental, the
  implementation will change in future.
* Logs and Metrics API & SDK - These are not compliant, and are behind feature
  flag. Not recommended to use for now.

## [1.0.0-rc4] 2021-08-04

* [EXPORTER] `BREAKING CHANGE` Fix default HTTP port for OTLP HTTP Exporter
  ([#939](https://github.com/open-telemetry/opentelemetry-cpp/pull/939))
* [API] Fix timeout calculation for Tracer::Flush() and Tracer::Close()
  ([#937](https://github.com/open-telemetry/opentelemetry-cpp/pull/937))
* [API] Fix for Stack::Resize that new_capacity is not assigned to capacity_
  ([#934](https://github.com/open-telemetry/opentelemetry-cpp/pull/934))
* [SDK] Fix race condition in circular buffer simulation test
  ([#931](https://github.com/open-telemetry/opentelemetry-cpp/pull/931))
* [EXPORTER] Fix error logging in Jaeger Exporter
  ([#930](https://github.com/open-telemetry/opentelemetry-cpp/pull/930))
* [BUILD] Use latest grpc version (v1.39.0) for cmake build of otlp exporter
  ([#927](https://github.com/open-telemetry/opentelemetry-cpp/pull/927))
* [EXPORTER] Add Jaeger Thrift HTTP exporter
  ([#926](https://github.com/open-telemetry/opentelemetry-cpp/pull/926))
* [SDK] Move env-var read logic to common dir, and optional reading of env-var
  ([#924](https://github.com/open-telemetry/opentelemetry-cpp/pull/924))
* [EXPORTER] Remove recordable header from CMake install rules for exporters
  ([#923](https://github.com/open-telemetry/opentelemetry-cpp/pull/923))
* [EXPORTER] `BREAKING CHANGE` Rename Recordable to JaegerRecordable in Jaeger
  exporter
  ([#919](https://github.com/open-telemetry/opentelemetry-cpp/pull/919))
* [EXPORTER] `BREAKING CHANGE` Rename Jaeger exporter target
  ([#918](https://github.com/open-telemetry/opentelemetry-cpp/pull/918))
* [EXPORTER] Add Zipkin exporter example
  ([#917](https://github.com/open-telemetry/opentelemetry-cpp/pull/917))
* [EXPORTER] Bazel build for Zipkin exorter
  ([#916](https://github.com/open-telemetry/opentelemetry-cpp/pull/916))
* [BUILD] Allow to use local GSL
  ([#915](https://github.com/open-telemetry/opentelemetry-cpp/pull/915))
* [DOCS] Document vcpkg toolchain configuration
  ([#913](https://github.com/open-telemetry/opentelemetry-cpp/pull/913))
* [SDK] Fix for resource deletion after tracer provider shutdown
  ([#911](https://github.com/open-telemetry/opentelemetry-cpp/pull/911))
* [BUILD] Add bazel build for grpc example
  ([#910](https://github.com/open-telemetry/opentelemetry-cpp/pull/910))
* [EXPORTER] Add resource and instrumentation library support for
  OStreamSpanExporter
  ([#906](https://github.com/open-telemetry/opentelemetry-cpp/pull/906))
* [API] Adding semantic-convention attributes for trace
  ([#868](https://github.com/open-telemetry/opentelemetry-cpp/pull/868))

## [1.0.0-rc3] 2021-07-12

* [DOCS] Add doxygen reference docs for SDK
  ([#902](https://github.com/open-telemetry/opentelemetry-cpp/pull/902))
* [EXPORTER] Jaeger Exporter - Populate resource attributes into process tags in
  Jaeger ([#897](https://github.com/open-telemetry/opentelemetry-cpp/pull/897))
* [EXPORTER] Zipkin Exporter - Report Event timestamp into microseconds
  ([#896](https://github.com/open-telemetry/opentelemetry-cpp/pull/896))
* [EXPORTER] Jaeger Exporter - Handle span events
  ([#895](https://github.com/open-telemetry/opentelemetry-cpp/pull/895))
* [DOCS] Fix API docs for Scope object
  ([#894](https://github.com/open-telemetry/opentelemetry-cpp/pull/894))
* [BUILD] Fix Linking error for libcurl on some macOS environments
  ([#893](https://github.com/open-telemetry/opentelemetry-cpp/pull/893))
* [API] Context cleanup from Runtime Storage
  ([#885](https://github.com/open-telemetry/opentelemetry-cpp/pull/885))
* [DOCS] Document strategy to avoid conflict between two different versions of
  Abseil ([#883](https://github.com/open-telemetry/opentelemetry-cpp/pull/883/))
* [EXPORTER] ETW Exporter - Document example for ETW Exporter
  ([#882](https://github.com/open-telemetry/opentelemetry-cpp/pull/882))
* [SDK] Create Span with Valid spanId and traceId irrespective of Sampling
  decision
  ([#879](https://github.com/open-telemetry/opentelemetry-cpp/pull/879))
* [EXPORTER] Jaeger Exporter - Rename bswap macros to avoid clash on some
  systems ([#876](https://github.com/open-telemetry/opentelemetry-cpp/pull/876))
* [API] Add Semantics Conventions attributes for Resources
  ([#872](https://github.com/open-telemetry/opentelemetry-cpp/pull/872))
* [BUILD] Use nlohmann-json from submodules if not already installed
  ([#870](https://github.com/open-telemetry/opentelemetry-cpp/pull/870))

## [1.0.0-rc2] 2021-06-18

* [EXPORTER] Jaeger Exporter - Support for Instrumentation Library
  ([#864](https://github.com/open-telemetry/opentelemetry-cpp/pull/864))
* [TESTS] Adding benchmark tests for baggage api
  ([#861](https://github.com/open-telemetry/opentelemetry-cpp/pull/861))
* [BUILD] Fix for GCC9/C++20 Support for using STL for modern C++ features
  ([#860](https://github.com/open-telemetry/opentelemetry-cpp/pull/860))
* [TESTS] Adding benchmark tests for span create api
  ([#856](https://github.com/open-telemetry/opentelemetry-cpp/pull/856))
* [BUILD] Fix for using Abseil library for modern C++ features
  ([#850](https://github.com/open-telemetry/opentelemetry-cpp/pull/850))
* [BUILD] Fix issues with win32/x86 compilation
  ([#847](https://github.com/open-telemetry/opentelemetry-cpp/pull/847))
* [DOCS] Document OSS dependencies and their licenses
  ([#844](https://github.com/open-telemetry/opentelemetry-cpp/pull/844))
* [BUILD] Various fixes to build with Visual Studio 2015
  ([#840](https://github.com/open-telemetry/opentelemetry-cpp/pull/840))
* [INSTRUMENTATION] HTTPClient: Change support for full URL argument
  ([#833](https://github.com/open-telemetry/opentelemetry-cpp/pull/833))
* [EXPORTER] Jaeger Exporter - fix endianness of Jaeger IDs for transmission
  ([#832](https://github.com/open-telemetry/opentelemetry-cpp/pull/832))
* [INSTRUMENTATION] fix protobuf compilation warning in gRPC example
  ([#830](https://github.com/open-telemetry/opentelemetry-cpp/pull/830))
* [EXPORTER] `BREAKING CHANGE` - Add OTLP/HTTP+JSON Protocol exporter; Rename
  `OtlpExporter` to `OtlpGrpcExporter`
  ([#810](https://github.com/open-telemetry/opentelemetry-cpp/pull/810))

## [1.0.0-rc1] 2021-06-04

* [BUILD] Enable Jaeger exporter build in Windows
  ([#815](https://github.com/open-telemetry/opentelemetry-cpp/pull/815))
* [DOCS] Versioning doc update to clarify release and versioning policy
  ([#811](https://github.com/open-telemetry/opentelemetry-cpp/pull/811))
* [LOGS] Move Logging implementation under feature-flag
  ([#807](https://github.com/open-telemetry/opentelemetry-cpp/pull/807))
* [BUILD] Filter metric headers files from `opentelemetry-api` and
  `opentelemetry-sdk` targets if metrics feature-flag is disabled
  ([#806](https://github.com/open-telemetry/opentelemetry-cpp/pull/806))
* [BUILD] Fix install rule for ostream exporter, Jaeger, ETW, ElasticSearch
  ([#805](Fix install rule for header files of ostream exporter))
* [API/SDK] Switch from mpark::variant to absl::variant as default
  ([#771](https://github.com/open-telemetry/opentelemetry-cpp/pull/771))
* [API/SDK] Support `const char *` as acceptable data type for attributes and
  resources
  ([#771](https://github.com/open-telemetry/opentelemetry-cpp/pull/771))
* [EXAMPLE] gRPC instrumentation example with context propagation
  ([#729](https://github.com/open-telemetry/opentelemetry-cpp/pull/729))

## [0.7.0] 2021-05-26

* [METRICS] Move metrics api/sdk under preview feature flag
  ([#745](https://github.com/open-telemetry/opentelemetry-cpp/pull/745))
* [DOCS] Add instructions to build using Bazel
  ([#747](https://github.com/open-telemetry/opentelemetry-cpp/pull/747))
* [DOCS] Update copyright headers
  ([#754](https://github.com/open-telemetry/opentelemetry-cpp/pull/754))
* [EXPORTER] Populate resource to OTLP proto data
  ([#758](https://github.com/open-telemetry/opentelemetry-cpp/pull/758))
* [CI] Add CodeQL security scan CI workflow
  ([#770](https://github.com/open-telemetry/opentelemetry-cpp/pull/770))
* [BUILD] Enable building API only CMake Project
  ([#778](https://github.com/open-telemetry/opentelemetry-cpp/pull/778))
* [SDK] Fix for sampling of root span
  ([#784](https://github.com/open-telemetry/opentelemetry-cpp/pull/784))
* [CI] Add Jaeger exporter to CMake CI build
  ([#786](https://github.com/open-telemetry/opentelemetry-cpp/pull/786))
* [API] `BREAKING CHANGE` - Tracer::WithActiveSpan() to return Scope object
  intead of unique_ptr
  ([#788](https://github.com/open-telemetry/opentelemetry-cpp/pull/788))
* [DOCS] Add docs for nested spans and context propagation in readthedocs
  ([#792](https://github.com/open-telemetry/opentelemetry-cpp/pull/792))
* [CI] Output verbose error for failed unit-test in CI
  ([#796](https://github.com/open-telemetry/opentelemetry-cpp/pull/796))

## [0.6.0] 2021-05-11

* [EXPORTER] Add Jaeger exporter
  ([#534](https://github.com/open-telemetry/opentelemetry-cpp/pull/534))
* [SDK] Support multiple processors
  ([#692](https://github.com/open-telemetry/opentelemetry-cpp/pull/692))
* [SDK] Add instrumentation library and multiple tracer support
  ([#693](https://github.com/open-telemetry/opentelemetry-cpp/pull/693))
* [SDK] Resource integration with Exporters
  ([#706](https://github.com/open-telemetry/opentelemetry-cpp/pull/706))
* [EXAMPLE] Enhance existing http example with propagation
  ([#727](https://github.com/open-telemetry/opentelemetry-cpp/pull/727))

## [0.5.0] 2021-04-26

* [SDK] Support custom span-id and trace-id generator
  ([#681](https://github.com/open-telemetry/opentelemetry-cpp/pull/681))
* [SDK] Add SpanContext (and TraceState) to Recordable
  ([#667](https://github.com/open-telemetry/opentelemetry-cpp/pull/667))
* [SDK] Global Propagator
  ([#668](https://github.com/open-telemetry/opentelemetry-cpp/pull/668))
* [SDK] Create SharedContext for updating span pipeline
  ([#650](https://github.com/open-telemetry/opentelemetry-cpp/pull/650))
* [API] Baggage implementation
  ([#676](https://github.com/open-telemetry/opentelemetry-cpp/pull/676))
* [API] Move class from opentelemetry::core namespace to opentelemetry::common
  namespace
  ([#686](https://github.com/open-telemetry/opentelemetry-cpp/pull/686))

## [0.4.0] 2021-04-12

* [EXPORTER] ETW Exporter enhancements
  ([#519](https://github.com/open-telemetry/opentelemetry-cpp/pull/519))
* [EXPORTER] Read Zipkin endpoint from environment variable.
  ([#624](https://github.com/open-telemetry/opentelemetry-cpp/pull/624))
* [EXPORTER] Split Zpages webserver hosting from Exporter
  ([#626](https://github.com/open-telemetry/opentelemetry-cpp/pull/626))
* [EXPORTER] ETW Exporter Usage Instructions
  ([#628](https://github.com/open-telemetry/opentelemetry-cpp/pull/628))
* [INSTRUMENTATION] HTTP Client/Server Instrumentation example
  ([#632](https://github.com/open-telemetry/opentelemetry-cpp/pull/632))
* [EXPORTER] Enable tls authentication for otlp grpc exporter ([#635](Enable tls
  authentication for otlp grpc exporter))
* [API] Refactoring trace_state to reuse common functionality in baggage
  ([#638](https://github.com/open-telemetry/opentelemetry-cpp/pull/638/files))

## [0.3.0] 2021-03-19

* [EXPORTER] Added Zipkin Exporter.
  ([#471](https://github.com/open-telemetry/opentelemetry-cpp/pull/471))
* [API] Added Jaeger propagator.
  ([#599](https://github.com/open-telemetry/opentelemetry-cpp/pull/599))
* [PROPAGATOR] Added Composite Propagator
  ([#597](https://github.com/open-telemetry/opentelemetry-cpp/pull/597))
* [API] Propagate traceflag from parent
  ([#603](https://github.com/open-telemetry/opentelemetry-cpp/pull/603))
* [DOCS] Add sphinx support for api doc generation
  ([595](https://github.com/open-telemetry/opentelemetry-cpp/pull/595))
* [SDK] Add service.name if missing in Resource
  ([#616](https://github.com/open-telemetry/opentelemetry-cpp/pull/616))

## [0.2.0] 2021-03-02

* [SDK] Added `ForceFlush` to `TracerProvider`.
  ([#588](https://github.com/open-telemetry/opentelemetry-cpp/pull/588)).
* [SDK] Added Resource API.
  ([#502](https://github.com/open-telemetry/opentelemetry-cpp/pull/502))
* [API] Modified TraceState support for w3c trace context as per specs.
* [SDK] TraceState implementation as per spec
  ([#551](https://github.com/open-telemetry/opentelemetry-cpp/pull/551))
* [API] Added B3 Propagator.
  ([#523](https://github.com/open-telemetry/opentelemetry-cpp/pull/523))
* [Exporter] Added ETW Exporter.
  ([#376](https://github.com/open-telemetry/opentelemetry-cpp/pull/376))
* [CI] Enable cache for Bazel for faster builds.
  ([#505](https://github.com/open-telemetry/opentelemetry-cpp/pull/505))

## [0.0.1] 2020-12-16

### Added

* Trace API and SDK experimental
* OTLP Exporter

### Changed

### Removed
