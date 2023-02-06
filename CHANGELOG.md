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

* Convert Prometheus Exporter to Pull MetricReader [#1953](https://github.com/open-telemetry/opentelemetry-cpp/pull/1953)
* Upgrade prometheus-cpp to v1.1.0 [#1954](https://github.com/open-telemetry/opentelemetry-cpp/pull/1954)

## [1.8.2] 2023-01-31

* Remove redundant macro check in nostd::shared_ptr [#1939](https://github.com/open-telemetry/opentelemetry-cpp/pull/1939)
* Fix typo in packages.cmake causing incorrect nuget package versions [#1936](https://github.com/open-telemetry/opentelemetry-cpp/pull/1936)
* [METRICS] Custom Aggregation support [#1899](https://github.com/open-telemetry/opentelemetry-cpp/pull/1899)
* Small fix in INSTALL.md for enabling building package. [#1930](https://github.com/open-telemetry/opentelemetry-cpp/pull/1930)
* [METRICS] Fix warning for misconfiguration of PeriodicExportingMetricReader [#1929](https://github.com/open-telemetry/opentelemetry-cpp/pull/1929)
* Make macros.h available for all source files via version.h [#1918](https://github.com/open-telemetry/opentelemetry-cpp/pull/1918)
* [METRICS] Histogram Aggregation: Fix bucket detection logic,
 performance improvements, and benchmark tests [#1869](https://github.com/open-telemetry/opentelemetry-cpp/pull/1869)
* Remove unused namespace alias for nostd [#1914](https://github.com/open-telemetry/opentelemetry-cpp/pull/1914)
* [METRICS] Update meter.h [#1907](https://github.com/open-telemetry/opentelemetry-cpp/pull/1907)
* sdk::resource::Resource::Merge should be const [#1905](https://github.com/open-telemetry/opentelemetry-cpp/pull/1905)
* [METRICS] Collect and Export metric data before
 PeriodicMetricReader shutdown. [#1860](https://github.com/open-telemetry/opentelemetry-cpp/pull/1860)
* [ETW EXPORTER] Add Virtual destructor for TailSampler, Update Maintainer
 mode warnings for MSVC [#1897](https://github.com/open-telemetry/opentelemetry-cpp/pull/1897)
* Fix #1867 Orderly shutdown in examples [#1868](https://github.com/open-telemetry/opentelemetry-cpp/pull/1868)
* [METRICS] minor metrics handling optimizations [#1890](https://github.com/open-telemetry/opentelemetry-cpp/pull/1890)
* fix SpinLockMutex for Intel Compiler [#1885](https://github.com/open-telemetry/opentelemetry-cpp/pull/1885)
* [LOGS] Change BatchLogRecordProcessorFactory::Create to static method [#1876](https://github.com/open-telemetry/opentelemetry-cpp/pull/1876)
* Enable generating deb, rpm, NuGet, tgz, zip package through cmake build [#1662](https://github.com/open-telemetry/opentelemetry-cpp/pull/1662)
* Updated clone command in INSTALL.md [#1818](https://github.com/open-telemetry/opentelemetry-cpp/pull/1818)
* Small cleanup to remove old metrics design docs [#1855](https://github.com/open-telemetry/opentelemetry-cpp/pull/1855)
* [BUILD] Fix build error with older version of VS2017 compiler. [1857](https://github.com/open-telemetry/opentelemetry-cpp/pull/1857)
* [EXPORTERS] Enable setting Span endtime for ETW exporter [#1846](https://github.com/open-telemetry/opentelemetry-cpp/pull/1846)
* [REMOVAL] Remove deprecated experimental semantic conventions [#1743](https://github.com/open-telemetry/opentelemetry-cpp/pull/1743)
* [EXPORTERS] Fix console debug logs for otlp exporters. [#1848](https://github.com/open-telemetry/opentelemetry-cpp/pull/1848)
* [LOGS] Add `include_trace_context` and `EventLogger` [#1884](https://github.com/open-telemetry/opentelemetry-cpp/pull/1884)
* [METRICS] Change BatchLogRecordProcessorFactory::Create to static method
* [BUILD] Fix OTELCPP_MAINTAINER_MODE [#1844](https://github.com/open-telemetry/opentelemetry-cpp/pull/1844)
* [BUILD] Fix compatibility when using clang and libc++, upgrade GTest and
  cmake when using C++20 [#1852](https://github.com/open-telemetry/opentelemetry-cpp/pull/1852)
* [SEMANTIC CONVENTIONS] Upgrade to version 1.16.0
  [#1854](https://github.com/open-telemetry/opentelemetry-cpp/pull/1854)
* [SDK] BatchSpanProcessor now logs a warning when dropping a span because the
  queue is full
  [1871](https://github.com/open-telemetry/opentelemetry-cpp/pull/1871)
* [BUILD] Migrate from @bazel_tools//platforms to [Bazel Platforms](https://github.com/bazelbuild/platforms)
  to enable Bazel 6.0.0 compatibility [#1873](https://github.com/open-telemetry/opentelemetry-cpp/pull/1873)
* [BUILD] Cleanup CMake makefiles for nlohmann_json
  [#1912](https://github.com/open-telemetry/opentelemetry-cpp/pull/1912)
* [BUILD] Cleanup CMake makefiles for CURL usage
  [#1916](https://github.com/open-telemetry/opentelemetry-cpp/pull/1916)
* [SEMANTIC CONVENTIONS] Upgrade to version 1.17.0
  [#1927](https://github.com/open-telemetry/opentelemetry-cpp/pull/1927)
* [MAINTAINER DOC] Define and document a deprecation process,
  [DEPRECATION] Deprecate the Jaeger exporter,
  implemented by [#1923](https://github.com/open-telemetry/opentelemetry-cpp/pull/1923)
* [BUILD] OTLP HTTP Exporter has build warnings in maintainer mode
  [#1943](https://github.com/open-telemetry/opentelemetry-cpp/pull/1943)

Deprecations:

* [MAINTAINER DOC] Define and document a deprecation process,
  [#1923](https://github.com/open-telemetry/opentelemetry-cpp/pull/1923)
  * A new file, [DEPRECATED](./DEPRECATED.md) list all the code currently
    deprecated.
  * A new [deprecation process](./docs/deprecation-process.md) details the plan to
    deprecate and later remove code.
* [DEPRECATION] Deprecate the Jaeger exporter
  [#1923](https://github.com/open-telemetry/opentelemetry-cpp/pull/1923)
  * The Jaeger Exporter is deprecated, see [DEPRECATED](./DEPRECATED.md) for details.

Important changes:

* [BUILD] Cleanup CMake makefiles for CURL usage
  [#1916](https://github.com/open-telemetry/opentelemetry-cpp/pull/1916)
  * CMake option `WITH_OTLP_HTTP`
    * Before this change, the CMake option `WITH_OTLP_HTTP` was unpredictable,
      sometime set to ON and sometime set to OFF by default,
      depending on whether a CURL package was found or not.
      The option `WITH_OTLP_HTTP` was sometime not displayed in the ccmake
      UI, making it impossible to even discover there is an option of that name.
    * With this change, CMake option `WITH_OTLP_HTTP` is always OFF by
      default. WITH_OTLP_HTTP MUST be set to ON explicitly to build the
      OTLP HTTP exporter. The option is always visible in the ccmake UI.
  * CMake option `BUILD_W3CTRACECONTEXT_TEST`
    * Before this change, the W3C trace context tests were built, or
      not, in an unpredictable way, depending on the presence, or not, of a
      CURL package. In particular, the build could ignore the W3C trace
      context tests even when BUILD_W3CTRACECONTEXT_TEST=ON.
    * With this change, option BUILD_W3CTRACECONTEXT_TEST is honored.
  * HTTP client/server examples
    * Before this change, the HTTP client/server examples were built, or
      not, in an unpredictable way, depending on the presence, or not, of a
      CURL package.
    * With this change, a new option `WITH_EXAMPLES_HTTP` is used to
      build the HTTP client/server examples.

## [1.8.1] 2022-12-04

* [ETW Exporter] Tail based sampling support [#1780](https://github.com/open-telemetry/opentelemetry-cpp/pull/1780)
* [EXPORTERS] fix typo [affecting otlp exported histogram metrics max uint] [#1827](https://github.com/open-telemetry/opentelemetry-cpp/pull/1827)
* [EXPORTERS] fix enum-compare-switch warning [#1833](https://github.com/open-telemetry/opentelemetry-cpp/pull/1833)
* [METRICS] Change default temporality as "Cumulative" for OTLP metrics
exporters [#1828](https://github.com/open-telemetry/opentelemetry-cpp/pull/1828)
* [BUILD] Moved otlp_grpc_utils.cc to opentelemetry_exporter_otlp_grpc_client.
[#1829](https://github.com/open-telemetry/opentelemetry-cpp/pull/1829)
* Fix type mismatch when move nostd::shared_ptr [#1815](https://github.com/open-telemetry/opentelemetry-cpp/pull/1815)
* [BUILD] Fix Prometheus target name [#1820](https://github.com/open-telemetry/opentelemetry-cpp/pull/1820)
* Clean unused docker files [#1817](https://github.com/open-telemetry/opentelemetry-cpp/pull/1817)
* [BUILD] Fix default bazel build [#1816](https://github.com/open-telemetry/opentelemetry-cpp/pull/1816)
* [BUILD] move client::nosend under test_common [#1811](https://github.com/open-telemetry/opentelemetry-cpp/pull/1811)
* [BUILD] Fix opentelemetry-proto file exists check [#1824](https://github.com/open-telemetry/opentelemetry-cpp/pull/1824)

## [1.8.0] 2022-11-27

* [DOC] Update Metrics status in README.md [#1722](https://github.com/open-telemetry/opentelemetry-cpp/pull/1722)
* [DOC] Remove misleading comments about ABI compatibility for nostd::span [#1731](https://github.com/open-telemetry/opentelemetry-cpp/pull/1731)
* [BUILD] Bump abseil-cpp for cmake CI [#1807](https://github.com/open-telemetry/opentelemetry-cpp/pull/1807)
* [Exporter] Add status code to OTLP grpc trace log [#1792](https://github.com/open-telemetry/opentelemetry-cpp/pull/1792)
* [Exporter] add fix for prometheus exporter build [#1795](https://github.com/open-telemetry/opentelemetry-cpp/pull/1795)
* [BUILD] Add option WITH_BENCHMARK to disable building benchmarks [#1794](https://github.com/open-telemetry/opentelemetry-cpp/pull/1794)
* [BUILD] Fix CI benchmark [#1799](https://github.com/open-telemetry/opentelemetry-cpp/pull/1799)
* [BUILD] bump to gRPC v1.48.1 for bazel CIs [#1786](https://github.com/open-telemetry/opentelemetry-cpp/pull/1786)
* [BUILD] Fix CI build [#1798](https://github.com/open-telemetry/opentelemetry-cpp/pull/1798)
* [BUILD] Fix clang-format in CI [#1796](https://github.com/open-telemetry/opentelemetry-cpp/pull/1796)
* Fix session lock of OtlpHttpClient [#1760](https://github.com/open-telemetry/opentelemetry-cpp/pull/1760)
* [Metrics SDK] Add MeterContext::ForEachMeter() method to process callbacks on
 Meter in thread-safe manner [#1783](https://github.com/open-telemetry/opentelemetry-cpp/pull/1783)
* [DOC] Document that clang-format version 10.0 is used. [#1782](https://github.com/open-telemetry/opentelemetry-cpp/pull/1782)
* [BUILD] Upgrade bazel build to use abseil-cpp-20220623.1 [#1779](https://github.com/open-telemetry/opentelemetry-cpp/pull/1779)
* Fix GlobalLogHandler singleton creation order [#1767](https://github.com/open-telemetry/opentelemetry-cpp/pull/1767)
* [Metrics SDK] Change Prometheus CMake target name [#1765](https://github.com/open-telemetry/opentelemetry-cpp/pull/1765)
* [DOC] Cleanup INSTALL.md [#1757](https://github.com/open-telemetry/opentelemetry-cpp/pull/1757)
* [DOC] Format config options in OTLP exporter readme [#1748](https://github.com/open-telemetry/opentelemetry-cpp/pull/1748)
* [DOC] Cleanup ENABLE_METRICS_PREVIEW [#1745](https://github.com/open-telemetry/opentelemetry-cpp/pull/1745)
* [Build] Multiple CURL packages leads to invalid build (#1738) [#1739](https://github.com/open-telemetry/opentelemetry-cpp/pull/1739)
* [Metrics SDK] Cleanup ENABLE_METRICS_PREVIEW [#1735](https://github.com/open-telemetry/opentelemetry-cpp/pull/1735)
* [Logs SDK] LogProcessor, LogExporter class name [#1736](https://github.com/open-telemetry/opentelemetry-cpp/pull/1736)
* [Metrics SDK] Cleanup of old _metric api/sdk [#1734](https://github.com/open-telemetry/opentelemetry-cpp/pull/1734)
* [ETW Exporter] Fix span timestamp(s) precision to nanoseconds [#1726](https://github.com/open-telemetry/opentelemetry-cpp/pull/1726)
* [LOGS SDK] Rename LogProcessor and LogExporter to LogRecordProcessor and LogRecordExporter
  [#1727](https://github.com/open-telemetry/opentelemetry-cpp/pull/1727)
* [METRICS SDK] - Remove old metrics from Github CI
  [#1733](https://github.com/open-telemetry/opentelemetry-cpp/pull/1733)
* [BUILD] Add CMake OTELCPP_PROTO_PATH [#1730](https://github.com/open-telemetry/opentelemetry-cpp/pull/1730)
* [SEMANTIC CONVENTIONS] Upgrade to version 1.15.0
  [#1761](https://github.com/open-telemetry/opentelemetry-cpp/pull/1761)
* [LOGS SDK] New LogRecord and logs::Recordable implementations.
  [#1766](https://github.com/open-telemetry/opentelemetry-cpp/pull/1766)

Deprecation notes:

* [Deprecation] Deprecate experimental semantic conventions
  [#1744](https://github.com/open-telemetry/opentelemetry-cpp/pull/1744)
  * The file
    `api/include/opentelemetry/trace/experimental_semantic_conventions.h`
    is deprecated, and will be removed in a future release.
    Use file
    `api/include/opentelemetry/trace/semantic_conventions.h`
    instead.

  * The file
    `sdk/include/opentelemetry/sdk/resource/experimental_semantic_conventions.h`
    is deprecated, and will be removed in a future release.
    Use file
    `sdk/include/opentelemetry/sdk/resource/semantic_conventions.h`
    instead.

  * The function, declared in the global namespace
    `uint32_t hashCode(const char *str, uint32_t h = 0)`
    is deprecated, and will be removed in a future release.
    No replacement will be provided.
    Note that function `opentelemetry::utils::hashCode`,
    declared in the ETW exporter, is not affected by this deprecation.

Breaking changes:

* [SEMANTIC CONVENTIONS] Upgrade to version 1.15.0
  [#1761](https://github.com/open-telemetry/opentelemetry-cpp/pull/1761)
  * Naming of semantic conventions has changed from uppercase constants,
    like `SemanticConventions::SERVICE_NAME`,
    to camel case, like `SemanticConventions::kServiceName`.
    This is necessary to avoid collisions with macros in general,
    which breaks the build on some platforms.
  * Semantic conventions are flagged as experimental,
    which is why this change is done in this release.

## [1.7.0] 2022-10-28

* [METRICS SDK] Validate Instrument meta data (name, unit, description) [#1713](https://github.com/open-telemetry/opentelemetry-cpp/pull/1713)
* [DOCS] Document libthrift 0.12.0 doesn't work with Jaeger exporter [#1714](https://github.com/open-telemetry/opentelemetry-cpp/pull/1714)
* [Metrics SDK] Add Monotonic Property to Sum Aggregation, and
unit tests for Up Down Counter [#1675](https://github.com/open-telemetry/opentelemetry-cpp/pull/1675)
* [Metrics SDK] Move Metrics Exemplar processing behind feature flag [#1710](https://github.com/open-telemetry/opentelemetry-cpp/pull/1710)
* [Metrics API/SDK] Change Meter API/SDK to return nostd::unique_ptr
 for Sync Instruments [#1707](https://github.com/open-telemetry/opentelemetry-cpp/pull/1707)
which includes breaking change in the Metrics api and sdk.
* [BUILD] Add e2e test to asan & tsan CI [#1670](https://github.com/open-telemetry/opentelemetry-cpp/pull/1670)
* [BUILD] Add otlp-grpc example bazel [#1708](https://github.com/open-telemetry/opentelemetry-cpp/pull/1708)
* [TRACE SDK] Fix debug log of OTLP HTTP exporter and ES log exporter [#1703](https://github.com/open-telemetry/opentelemetry-cpp/pull/1703)
* [METRICS SDK] Fix a potential precision loss on integer in
ReservoirCellIndexFor [#1696](https://github.com/open-telemetry/opentelemetry-cpp/pull/1696)
* [METRICS SDK] Fix Histogram crash [#1685](https://github.com/open-telemetry/opentelemetry-cpp/pull/1685)
* [METRICS SDK] Fix:1676 Segfault when short export period is used for metrics [#1682](https://github.com/open-telemetry/opentelemetry-cpp/pull/1682)
* [METRICS SDK] Add timeout support to MeterContext::ForceFlush [#1673](https://github.com/open-telemetry/opentelemetry-cpp/pull/1673)
* [DOCS] - Minor updates to OStream Metrics exporter documentation [#1679](https://github.com/open-telemetry/opentelemetry-cpp/pull/1679)
* [DOCS] Fix:#1575 API Documentation for Metrics SDK and API [#1678](https://github.com/open-telemetry/opentelemetry-cpp/pull/1678)
* [BUILD] Fixed compiler warnings [#1677](https://github.com/open-telemetry/opentelemetry-cpp/pull/1677)
* [METRICS SDK] Fix threading issue between Meter::RegisterSyncMetricStorage
 and Meter::Collect [#1666](https://github.com/open-telemetry/opentelemetry-cpp/pull/1666)
* [METRICS SDK] Fix data race on MeterContext::meters_ [#1668](https://github.com/open-telemetry/opentelemetry-cpp/pull/1668)
* [METRICS SDK] Fix observable Gauge metrics generation [#1651](https://github.com/open-telemetry/opentelemetry-cpp/pull/1651)
* [BUILD] Detect ARCH=sparc in CMake [#1660](https://github.com/open-telemetry/opentelemetry-cpp/pull/1660)
* [SDK] Add user agent for OTLP http/grpc client [#1657](https://github.com/open-telemetry/opentelemetry-cpp/pull/1657)
* [BUILD] Fix clang and gcc warnings [#1658](https://github.com/open-telemetry/opentelemetry-cpp/pull/1658)
* [Metrics SDK] Add Metrics ExemplarFilter and ExemplarReservoir [#1584](https://github.com/open-telemetry/opentelemetry-cpp/pull/1584)
* [LOGS SDK] Rename OnReceive to OnEmit [#1652](https://github.com/open-telemetry/opentelemetry-cpp/pull/1652)
* [METRICS SDK] Fix Observable Gauge does not reflect updated values,
and send the old value always [#1641](https://github.com/open-telemetry/opentelemetry-cpp/pull/1641)
* [Metrics SDK] Change boundary type to double for Explicit Bucket Histogram Aggregation,
and change default bucket range [#1626](https://github.com/open-telemetry/opentelemetry-cpp/pull/1626)
* [METRICS SDK] Fix occasional Segfault with LongCounter instrument [#1638](https://github.com/open-telemetry/opentelemetry-cpp/pull/1638)
* [BUILD] Bump vcpk to 2022.08.15 [#1633](https://github.com/open-telemetry/opentelemetry-cpp/pull/1633)
* [BUILD] Bump gRPC to v1.48.1 for CMake Linux CI [#1608](https://github.com/open-telemetry/opentelemetry-cpp/pull/1608)
* [Metrics] Switch to explicit 64 bit integers [#1686](https://github.com/open-telemetry/opentelemetry-cpp/pull/1686)
  which includes breaking change in the Metrics api and sdk.
* [Metrics SDK] Add support for Pull Metric Exporter [#1701](https://github.com/open-telemetry/opentelemetry-cpp/pull/1701)
  which includes breaking change in the Metrics api.
* [BUILD] Add CMake OTELCPP_MAINTAINER_MODE [#1650](https://github.com/open-telemetry/opentelemetry-cpp/pull/1650)
* [SEMANTIC CONVENTIONS] Upgrade to version 1.14.0 [#1697](https://github.com/open-telemetry/opentelemetry-cpp/pull/1697)

Notes:

Metrics API/SDK GA release includes PRs [#1686](https://github.com/open-telemetry/opentelemetry-cpp/pull/1686),
[#1701](https://github.com/open-telemetry/opentelemetry-cpp/pull/1701), and
[#1707](https://github.com/open-telemetry/opentelemetry-cpp/pull/1707)
with breaking changes in the Metrics API and SDK.

## [1.6.1] 2022-09-22

* [BUILD] Upgrade opentelemetry-proto to v0.19.0 [#1579](https://github.com/open-telemetry/opentelemetry-cpp/pull/1579)
* [METRICS EXPORTER] Add `OtlpGrpcMetricExporterFactory` and `OtlpHttpMetricExporterFactory`.
  [#1606](https://github.com/open-telemetry/opentelemetry-cpp/pull/1606)
* [SEMANTIC CONVENTIONS] Upgrade to version 1.13.0 [#1624](https://github.com/open-telemetry/opentelemetry-cpp/pull/1624)
* [BUILD] Fixes span creation benchmark issue. [#1622](https://github.com/open-telemetry/opentelemetry-cpp/pull/1622)
* [BUILD] Fix more build warnings (#1616) [#1620](https://github.com/open-telemetry/opentelemetry-cpp/pull/1620)
* [SDK gRPC]: Fix out-of-bounds access of string_view in GrpcClientCarrier in
      the example
      [#1619](https://github.com/open-telemetry/opentelemetry-cpp/pull/1619)
* [EXPORTER ETW] Add Trace flags in SpanContext [#1618](https://github.com/open-telemetry/opentelemetry-cpp/pull/1618)
* [SDK] resource sdk: Update Resource::Merge function docs [#1615](https://github.com/open-telemetry/opentelemetry-cpp/pull/1615)
* [BUILD] Fix build warnings [#1613](https://github.com/open-telemetry/opentelemetry-cpp/pull/1613)
* [API BUILD] Fix header only api singletons (#1520) [#1604](https://github.com/open-telemetry/opentelemetry-cpp/pull/1604)
* [METRICS SDK] Fix default value of
  `OtlpHttpMetricExporterOptions::aggregation_temporality`.
  [#1601](https://github.com/open-telemetry/opentelemetry-cpp/pull/1601)
* [METRICS EXAMPLE] Example for OTLP gRPC exporter for Metrics. [#1598](https://github.com/open-telemetry/opentelemetry-cpp/pull/1598)
* [SDK] Fix `LoggerContext::Shutdown` and tsan of `OtlpHttpClient` [#1592](https://github.com/open-telemetry/opentelemetry-cpp/pull/1592)
* [METRICS SDK] Fix 1585 - Multiple cumulative metric collections without
  measurement recording.
  [#1586](https://github.com/open-telemetry/opentelemetry-cpp/pull/1586)
* [BUILD] metrics warnings [#1583](https://github.com/open-telemetry/opentelemetry-cpp/pull/1583)
* [METRICS SDK] Fix ObservableInstrument::RemoveCallback [#1582](https://github.com/open-telemetry/opentelemetry-cpp/pull/1582)
* [SDK] Add error log when getting a http error code [#1581](https://github.com/open-telemetry/opentelemetry-cpp/pull/1581)
* [EXPORTER] ETW Exporter - Add support for Sampler and ID Generator [#1547](https://github.com/open-telemetry/opentelemetry-cpp/pull/1547)

Notes:

While [OpenTelemetry semantic
convention](https://github.com/open-telemetry/opentelemetry-specification/tree/main/semantic_conventions)
is still in experimental state, PR
[#1624](https://github.com/open-telemetry/opentelemetry-cpp/pull/1624) upgraded
it from 1.12.0 to 1.13.0 which **MAY** break the instrumentation library. Please
update the semantic convention in instrumentation library is needed.

## [1.6.0] 2022-08-15

* [METRICS SDK] Calling Observable Instruments callback during metrics
  collection
  [#1554](https://github.com/open-telemetry/opentelemetry-cpp/pull/1554)
* [METRICS CI] Add CI jobs for new and deprecated metrics [#1531](https://github.com/open-telemetry/opentelemetry-cpp/pull/1531)
* [METRICS BUILD] Fix metrics asan and tsan CI [#1562](https://github.com/open-telemetry/opentelemetry-cpp/pull/1562)
* [METRICS SDK] remove throw check from metrics  with noexcept [#1560](https://github.com/open-telemetry/opentelemetry-cpp/pull/1560)
* [METRICS SDK] fix metrics race condition [#1552](https://github.com/open-telemetry/opentelemetry-cpp/pull/1552)
* [METRICS SDK] Fix metrics context circular reference [#1535](https://github.com/open-telemetry/opentelemetry-cpp/pull/1535)
* [METRICS EXPORTER] Improve scope/instrument names in metrics ostream exporter [#1544](https://github.com/open-telemetry/opentelemetry-cpp/pull/1544)
* [METRICS BUILD] fix IWYU error in instruments.h [#1555](https://github.com/open-telemetry/opentelemetry-cpp/pull/1555)
* [EXPORTER] Prometheus exporter support Gauge Type [#1553](https://github.com/open-telemetry/opentelemetry-cpp/pull/1553)
* [METRICS SDK] Fix default Metric view name [#1515](https://github.com/open-telemetry/opentelemetry-cpp/pull/1515)
* [SDK] Fix infinitely waiting when shutdown with more than one running http
  sessions.
  [#1549](https://github.com/open-telemetry/opentelemetry-cpp/pull/1549)
* [METRICS SDK] Fix OTLP gRPC Metrics env variables [#1543](https://github.com/open-telemetry/opentelemetry-cpp/pull/1543)
* [METRICS SDK] Metric aggregation temporality controls  [#1541](https://github.com/open-telemetry/opentelemetry-cpp/pull/1541)
* [METRICS SDK] Histogram min/max support [#1540](https://github.com/open-telemetry/opentelemetry-cpp/pull/1540)
* [METRICS EXPORTER] ostream exporter should print out resource attributes [#1523](https://github.com/open-telemetry/opentelemetry-cpp/pull/1523)
* [METRICS SDK] Support multiple async callbacks [#1495](https://github.com/open-telemetry/opentelemetry-cpp/pull/1495)
* [BUILD] Fix code scanning alert [#1530](https://github.com/open-telemetry/opentelemetry-cpp/pull/1530)
* [BUILD] Fix several compiling/linking errors [#1539](https://github.com/open-telemetry/opentelemetry-cpp/pull/1539)
* [TRACE SDK] Add SpanData getter for Span Recordable [#1508](https://github.com/open-telemetry/opentelemetry-cpp/pull/1508)
* [LOG SDK] Fix log sdk builder (#1486) [#1524](https://github.com/open-telemetry/opentelemetry-cpp/pull/1524)
* [METRICS SDK] Add configuration options for Aggregation creation [#1513](https://github.com/open-telemetry/opentelemetry-cpp/pull/1513)
* [METRICS TEST] Fix metrics unit test memory leack [#1533](https://github.com/open-telemetry/opentelemetry-cpp/pull/1533)
* [LOG SDK] Add log sdk builders (#1486) [#1524](https://github.com/open-telemetry/opentelemetry-cpp/pull/1524)

## [1.5.0] 2022-07-29

* [EXPORTER BUILD] Add resources to dep list of prometheus exporter test [#1527](https://github.com/open-telemetry/opentelemetry-cpp/pull/1527)
* [BUILD] Don't require applications using jaeger exporter to know about libcurl
  [#1518](https://github.com/open-telemetry/opentelemetry-cpp/pull/1518)
* [EXPORTER] Inline print_value() in ostream exporter [#1512](https://github.com/open-telemetry/opentelemetry-cpp/pull/1512)
* [SDK] fix: urlPaser will incorrect parsing url like "http://abc.com/xxx@xxx/a/b"
  [#1511](https://github.com/open-telemetry/opentelemetry-cpp/pull/1511)
* [SDK] Rename `InstrumentationLibrary` to `InstrumentationScope` [#1507](https://github.com/open-telemetry/opentelemetry-cpp/pull/1507)
* [BUILD] Try to build nlohmann-json only it's depended. [#1505](https://github.com/open-telemetry/opentelemetry-cpp/pull/1505)
* [EXPORTER BUILD] Link opentelemetry_api to ETW exporter test [#1503](https://github.com/open-telemetry/opentelemetry-cpp/pull/1503)
* [SDK] Add automatically semantic conventions from the spec (#873) [#1497](https://github.com/open-telemetry/opentelemetry-cpp/pull/1497)
* [SDK] Use template class for in-memory data. [#1496](https://github.com/open-telemetry/opentelemetry-cpp/pull/1496)
* [SDK] fix compiler warnings [#1499](https://github.com/open-telemetry/opentelemetry-cpp/pull/1499)
* [TRACE SDK] Add trace sdk builders (#1393) [#1471](https://github.com/open-telemetry/opentelemetry-cpp/pull/1471)
* [METRICS BUILD] Enable bazel build for metrics proto files [#1489](https://github.com/open-telemetry/opentelemetry-cpp/pull/1489)
* [METRICS EXPORTER] Add metrics OTLP/HTTP exporter [#1487](https://github.com/open-telemetry/opentelemetry-cpp/pull/1487)
* [EXPORTER] fix otlp grpc exporter naming [#1488](https://github.com/open-telemetry/opentelemetry-cpp/pull/1488)
* [BUILD] Remove `--config Debug` when installing. [#1480](https://github.com/open-telemetry/opentelemetry-cpp/pull/1480)
* [EXPORTER] Fix endpoint in otlp grpc exporter [#1474](https://github.com/open-telemetry/opentelemetry-cpp/pull/1474)
* [EXAMPLE] Fix memory ownership of InMemorySpanExporter (#1473) [#1471](https://github.com/open-telemetry/opentelemetry-cpp/pull/1471)
* [EXPORTER TESTS] Prometheus unit test [#1461](https://github.com/open-telemetry/opentelemetry-cpp/pull/1461)
* [DOCS] Update docs to use relative code links. [#1447](https://github.com/open-telemetry/opentelemetry-cpp/pull/1447)
* [SDK] Remove reference to deprecated InstrumentationLibrary in OTLP [#1469](https://github.com/open-telemetry/opentelemetry-cpp/pull/1469)
* [SDK] Fix trace kIsSampled flag set incorrectly [#1465](https://github.com/open-telemetry/opentelemetry-cpp/pull/1465)
* [METRICS EXPORTER] OTLP gRPC Metrics Exporter [#1454](https://github.com/open-telemetry/opentelemetry-cpp/pull/1454)
* [EXPORTER] fix prometheus exporter failure type [#1460](https://github.com/open-telemetry/opentelemetry-cpp/pull/1460)
* [SDK] Fix build warnings about missing aggregates. [#1368](https://github.com/open-telemetry/opentelemetry-cpp/pull/1368)
* [EXT] `curl::HttpClient` use `curl_multi_handle` instead of creating a thread
  for every request and it's able to reuse connections now. ([#1317](https://github.com/open-telemetry/opentelemetry-cpp/pull/1317))
* [SEMANTIC CONVENTIONS] Upgrade to version 1.12.0 [#873](https://github.com/open-telemetry/opentelemetry-cpp/pull/873)

## [1.4.1] 2022-06-19

* [METRICS SDK] Fix variables inizialization [#1430](https://github.com/open-telemetry/opentelemetry-cpp/pull/1430)
* [DOCS] Fixed broken link to OpenTelemetry.io (#1445) [#1446](https://github.com/open-telemetry/opentelemetry-cpp/pull/1446)
* [BUILD] Upgrade nlohmann_json to 3.10.5 (#1438) [#1441](https://github.com/open-telemetry/opentelemetry-cpp/pull/1441)
* [METRICS SDK] fix histogram [#1440](https://github.com/open-telemetry/opentelemetry-cpp/pull/1440)
* [DOCS] Fix GettingStarted documentation for Jaeger HTTP exporter (#1347) [#1439](https://github.com/open-telemetry/opentelemetry-cpp/pull/1439)
* [BUILD] install sdk-config.h [#1419](https://github.com/open-telemetry/opentelemetry-cpp/pull/1419)
* [EXAMPLE] Log current timestamp instead of epoch time [#1434](https://github.com/open-telemetry/opentelemetry-cpp/pull/1434)
* [METRICS SDK] Add attributes/dimensions to metrics ostream exporter [#1400](https://github.com/open-telemetry/opentelemetry-cpp/pull/1400)
* [SDK] Fix global log handle symbols when using dlopen [#1420](https://github.com/open-telemetry/opentelemetry-cpp/pull/1420)
* [METRICS] Only record non-negative / finite / Non-NAN histogram values([#1427](https://github.com/open-telemetry/opentelemetry-cpp/pull/1427))
* [ETW EXPORTER] Fix ETW log exporter header inclusion [#1426](https://github.com/open-telemetry/opentelemetry-cpp/pull/1426)
* [ETW EXPORTER] Copy string_view passed to ETW exporter in PropertyVariant [#1425](https://github.com/open-telemetry/opentelemetry-cpp/pull/1425)
* [METRICS API/SDK] Pass state to async callback function. [#1408](https://github.com/open-telemetry/opentelemetry-cpp/pull/1408)
* [BUILD] fix nlohmann_json's (third party) include dir [#1415](https://github.com/open-telemetry/opentelemetry-cpp/pull/1415)
* [SDK] fix: WaitOnSocket select error when sockfd above FD_SETSIZE [#1410](https://github.com/open-telemetry/opentelemetry-cpp/pull/1410)
* [SDK] fix OTEL_INTERNAL_LOG_INFO [#1407](https://github.com/open-telemetry/opentelemetry-cpp/pull/1407)
* [DOCS] Document Getting Started with Prometheus and Grafana [#1396](https://github.com/open-telemetry/opentelemetry-cpp/pull/1396)

## [1.4.0] 2022-05-17

* [API SDK] Upgrade proto to v0.17.0, update log data model ([#1383](https://github.com/open-telemetry/opentelemetry-cpp/pull/1383))
* [BUILD] Alpine image ([#1382](https://github.com/open-telemetry/opentelemetry-cpp/pull/1382))
* [LOGS SDK] Get span_id from context when Logger::Log received invalid span_id
  ([#1398](https://github.com/open-telemetry/opentelemetry-cpp/pull/1398))
* [METRICS SDK] Connect async storage with async instruments ([#1388](https://github.com/open-telemetry/opentelemetry-cpp/pull/1388))
* [DOCS] Getting started document using ostream exporter ([#1394](https://github.com/open-telemetry/opentelemetry-cpp/pull/1394))
* [BUILD] Fix missing link to nlohmann_json ([#1390](https://github.com/open-telemetry/opentelemetry-cpp/pull/1390))
* [SDK] Fix sharing resource in batched exported spans ([#1386](https://github.com/open-telemetry/opentelemetry-cpp/pull/1386))
* [PROTOCOL \& LOGS] Upgrade proto to v0.17.0, update log data model ([#1383](https://github.com/open-telemetry/opentelemetry-cpp/pull/1383))
* [METRICS SDK] Remove un-necessary files. ([#1379](https://github.com/open-telemetry/opentelemetry-cpp/pull/1379))
* [EXPORTER] Prometheus exporter meters and instrument name ([#1378](https://github.com/open-telemetry/opentelemetry-cpp/pull/1378))
* [API] Add noexcept/const qualifier at missing places for Trace API. ([#1374](https://github.com/open-telemetry/opentelemetry-cpp/pull/1374))
* [SDK] Fix empty tracestate header propagation ([#1373](https://github.com/open-telemetry/opentelemetry-cpp/pull/1373))
* [METRICS SDK] Reuse temporal metric storage for sync storage ([#1369](https://github.com/open-telemetry/opentelemetry-cpp/pull/1369))
* [SDK] Fix baggage propagation for empty/invalid baggage context ([#1367](https://github.com/open-telemetry/opentelemetry-cpp/pull/1367))
* [BUILD] Export opentelemetry_otlp_recordable ([#1365](https://github.com/open-telemetry/opentelemetry-cpp/pull/1365))
* [TESTS] Disable test on prometheus-cpp which not need ([#1363](https://github.com/open-telemetry/opentelemetry-cpp/pull/1363))
* [METRICS] Fix class member initialization order ([#1360](https://github.com/open-telemetry/opentelemetry-cpp/pull/1360))
* [METRICS SDK] Simplify SDK Configuration: Use View with default aggregation if
  no matching View is configured
  ([#1358](https://github.com/open-telemetry/opentelemetry-cpp/pull/1358))
* [BUILD] Add missing include guard ([#1357](https://github.com/open-telemetry/opentelemetry-cpp/pull/1357))
* [ETW EXPORTER] Fix scalar delete against array ([#1356](https://github.com/open-telemetry/opentelemetry-cpp/pull/1356))
* [ETW EXPORTER] Conditional include for codecvt header ([#1355](https://github.com/open-telemetry/opentelemetry-cpp/pull/1355))
* [BUILD] Use latest TraceLoggingDynamic.h ([#1354](https://github.com/open-telemetry/opentelemetry-cpp/pull/1354))
* [SDK] Add explicit type cast in baggage UrlDecode ([#1353](https://github.com/open-telemetry/opentelemetry-cpp/pull/1353))
* [METRICS SDK] Remove exporter registration to meter provider ([#1350](https://github.com/open-telemetry/opentelemetry-cpp/pull/1350))
* [METRICS SDK] Fix output time in metrics OStream exporter ([#1346](https://github.com/open-telemetry/opentelemetry-cpp/pull/1346))
* [BUILD] ostream metrics cmake ([#1344](https://github.com/open-telemetry/opentelemetry-cpp/pull/1344))
* [BUILD] Link `opentelemetry_ext` with `opentelemetry_api` ([#1336](https://github.com/open-telemetry/opentelemetry-cpp/pull/1336))
* [METRICS SDK] Enable metric collection for Async Instruments - Delta and
  Cumulative
  ([#1334](https://github.com/open-telemetry/opentelemetry-cpp/pull/1334))
* [BUILD] Dependencies image as artifact ([#1333](https://github.com/open-telemetry/opentelemetry-cpp/pull/1333))
* [EXAMPLE] Prometheus example ([#1332](https://github.com/open-telemetry/opentelemetry-cpp/pull/1332))
* [METRICS EXPORTER] Prometheus exporter ([#1331](https://github.com/open-telemetry/opentelemetry-cpp/pull/1331))
* [METRICS] Metrics histogram example ([#1330](https://github.com/open-telemetry/opentelemetry-cpp/pull/1330))
* [TESTS] Replace deprecated googletest API ([#1327](https://github.com/open-telemetry/opentelemetry-cpp/pull/1327))
* [BUILD] Fix Ninja path ([#1326](https://github.com/open-telemetry/opentelemetry-cpp/pull/1326))
* [API] Update yield logic for ARM processor ([#1325](https://github.com/open-telemetry/opentelemetry-cpp/pull/1325))
* [BUILD] Fix metrics compiler warnings ([#1328](https://github.com/open-telemetry/opentelemetry-cpp/pull/1328))
* [METRICS SDK] Implement Merge and Diff operation for Histogram Aggregation ([#1303](https://github.com/open-telemetry/opentelemetry-cpp/pull/1303))

Notes:

While opentelemetry-cpp Logs are still in experimental stage,
[#1383](https://github.com/open-telemetry/opentelemetry-cpp/pull/1383) updated
opentelemetry-proto to 0.17.0, which includes some breaking change in the
protocol, like
[this](https://github.com/open-telemetry/opentelemetry-proto/pull/373). This
makes `name` parameter for our log API unnecessary. However, this parameter is
marked deprecated instead of being removed in this release, and it will be
removed in future release.

## [1.3.0] 2022-04-11

* [ETW EXPORTER] ETW provider handle cleanup ([#1322](https://github.com/open-telemetry/opentelemetry-cpp/pull/1322))
* [BUILD] Move public definitions into `opentelemetry_api`. ([#1314](https://github.com/open-telemetry/opentelemetry-cpp/pull/1314))
* [METRICS] OStream example ([#1312](https://github.com/open-telemetry/opentelemetry-cpp/pull/1312))
* [BUILD] Rename `http_client_curl` to `opentelemetry_http_client_curl` ([#1301](https://github.com/open-telemetry/opentelemetry-cpp/pull/1301))
* [METRICS SDK] Add InstrumentationInfo and Resource to the metrics data to be
  exported.
  ([#1299](https://github.com/open-telemetry/opentelemetry-cpp/pull/1299))
* [TESTS] Add building test without RTTI ([#1294](https://github.com/open-telemetry/opentelemetry-cpp/pull/1294))
* [METRICS SDK] Implement periodic exporting metric reader ([#1286](https://github.com/open-telemetry/opentelemetry-cpp/pull/1286))
* [SDK] Bugfix: span SetAttribute crash ([#1283](https://github.com/open-telemetry/opentelemetry-cpp/pull/1283))
* [BUG] Remove implicitly deleted default constructor ([#1267](https://github.com/open-telemetry/opentelemetry-cpp/pull/1267))
* [METRICS SDK] Synchronous Metric collection (Delta , Cumulative) ([#1265](https://github.com/open-telemetry/opentelemetry-cpp/pull/1265))
* [METRICS SDK] Metrics exemplar round 1 ([#1264](https://github.com/open-telemetry/opentelemetry-cpp/pull/1264))
* [EXPORTER] Fix: use CURLOPT_TIMEOUT_MS to config OtlpHttpExporter's timeout
  instead of CURLOPT_TIMEOUT
  ([#1261](https://github.com/open-telemetry/opentelemetry-cpp/pull/1261))
* [EXPORTER] Jaeger Exporter - Populate Span Links ([#1251](https://github.com/open-telemetry/opentelemetry-cpp/pull/1251))
* [SDK] Reorder the destructor of members in LoggerProvider and TracerProvider ([#1245](https://github.com/open-telemetry/opentelemetry-cpp/pull/1245))
* [METRICS SDK] Enable metric collection from MetricReader ([#1241](https://github.com/open-telemetry/opentelemetry-cpp/pull/1241))
* [METRICS SDK] Asynchronous Aggregation storage ([#1232](https://github.com/open-telemetry/opentelemetry-cpp/pull/1232))
* [METRICS SDK] Synchronous Instruments - Aggregation Storage(s) creation for
  configured views
  ([#1219](https://github.com/open-telemetry/opentelemetry-cpp/pull/1219))
* [BUILD] Added s390x arch into CMake build. ([#1216](https://github.com/open-telemetry/opentelemetry-cpp/pull/1216))
* [API] Allow extension of the lifetime of ContextStorage. ([#1214](https://github.com/open-telemetry/opentelemetry-cpp/pull/1214))
* [METRICS SDK] Add Aggregation storage ([#1213](https://github.com/open-telemetry/opentelemetry-cpp/pull/1213))
* [TESTS] Fix ostream_log_test Mac ([#1208](https://github.com/open-telemetry/opentelemetry-cpp/pull/1208))
* [BUILD] Update grpc to v1.43.2 to support VS2022/MSVC 19.30 and bazel 5.0 ([#1207](https://github.com/open-telemetry/opentelemetry-cpp/pull/1207))
* [DOCS] Benchmark documentation ([#1205](https://github.com/open-telemetry/opentelemetry-cpp/pull/1205))
* [DOCS] Fix errors in SDK documentation ([#1201](https://github.com/open-telemetry/opentelemetry-cpp/pull/1201))
* [METRICS EXPORTER] Ostream metric exporter ([#1196](https://github.com/open-telemetry/opentelemetry-cpp/pull/1196))
* [Metrics SDK] Filtering metrics attributes ([#1191](https://github.com/open-telemetry/opentelemetry-cpp/pull/1191))
* [Metrics SDK] Sync and Async Instruments SDK ([#1184](https://github.com/open-telemetry/opentelemetry-cpp/pull/1184))
* [Metrics SDK] Add Aggregation as part of metrics SDK. ([#1178](https://github.com/open-telemetry/opentelemetry-cpp/pull/1178))
* [BUILD] Cmake: thrift requires boost headers, include them as
  Boost_INCLUDE_DIRS
  ([#1100](https://github.com/open-telemetry/opentelemetry-cpp/pull/1100))

Notes:

[#1301](https://github.com/open-telemetry/opentelemetry-cpp/pull/1301) added
`opentelemetry_` as prefix to http_client_curl library for resolving potential
naming conflict, this could break existing cmake build if http_client_curl is
listed as explicit dependency in user's cmake file.

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
