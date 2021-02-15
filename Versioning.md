# Versioning

This document describes the versioning policy for this repository.

## Goals

### API Stability

Once the API for a given signal (spans, logs, metrics, baggage) has been
officially released, that API module will function with any SDK that has the
same MAJOR version and equal or greater MINOR or PATCH version. For example,
application compiled with API v1.1 is compatible with SDK v1.1.2, v1.2.0, etc.

For example, libraries that are instrumented with `opentelemetry 1.0.1` will
function in applications using `opentelemetry 1.11.33` or `opentelemetry 1.3.4`.

### SDK Stability

Public portions of the SDK (constructors, configuration, end-user interfaces)
must remain backwards compatible. Internal types are allowed to break.

### ABI Stability

Refer to the [ABI Policy](./docs/abi-policy.md) for more details. To summarise:

* ABI stability is guaranteed for the API.
* ABI stability is not guaranteed for the SDK. In case of ABI breaking changes,
  instead of bumping up the major version, a new `inline namespace` version will
  be created, and both old API and new API would be made available
  simultaneously.

## Policy

* Release versions will follow [SemVer 2.0](https://semver.org/).
* Only a single source package containing both the api and sdk for all signals
  will be released as part of each GitHub release.
* There will be source package releases for api and sdk. There won't be separate
  releases for the signals. The release version numbers for api and sdk will not
  be in sync with each other. As there would be more frequent changes expected
  in sdk than in the api.
* Experimental releases: New (unstable) telemetry signals and features will be
  introduced behind feature flag protected by a preprocessor macro.

  ```cpp
  #ifdef FEATURE_FLAG
    <metrics api/sdk definitions>
  #endif
  ```

  As we deliver the package in source form, and the user is responsible to build
  it for their platform, the user must be aware of these feature flags
  (documented in the [CHANGELOG.md](CHANGELOG.md) file). The user must enable
  them explicitly through their build system (CMake, Bazel or others) to use any
  preview features.

  The guidelines in creating feature flag would be:

  * Naming:
    * `ENABLE_<SIGNAL>_PREVIEW` : For experimental release of signal api/sdks
      eg, `METRICS_PREVIEW`, `LOGGING_PREVIEW`,
    * `ENABLE_<SIGNAL>_<FEATURE_NAME>_PREVIEW` : For experimental release for
      any feature within stable signal. For example, `TRACING_JAEGER_PREVIEW` to
      release the experimental Jaeger exporter for tracing.
  * Cleanup: It is good practice to keep feature-flags as shortlived as
    possible. And, also important to keep the number of them low. They should be
    used such that it is easy to remove/cleanup them once the experimental
    feature is stable.

* New signals will be stabilized via a **minor version bump**, and are not
  allowed to break existing stable interfaces. Feature flags will be removed
  once we have a stable implementation for the signal.

* GitHub releases will be made for all released versions.

## Example Versioning Lifecycle

Purely for illustration purposes, not intended to represent actual releases:

* v0.0.1 release:
  * `opentelemetry-api 0.0.1`
    * Contains experimental api's of trace, resouce ( no feature flag as major
      version is 0 )
    * No api's of logging and metrics available
  * `opentelemetry-sdk 0.0.1`
    * Contains experimental implementation of trace, resouce ( no feature flag
      as major version is 0 )
    * No implemtation of logging and metrics available
* v1.0.0 release: ( with traces )
  * `opentelemetry-api 1.0.0`
    * Contains stable apis of trace, baggage and  resource
    * experimental metrics api's behind feature flag
  * `opentelemetry-sdk 1.0.0`
    * Contains stable implementation of trace, baggage and  resource
    * experimental metrics api's behind feature flag
* v1.5.0 release (with metrics)
  * `opentelemetry-api 1.5.0`
    * Contains stable api's of metrics, trace, baggage, resource, context
      modules
    * experimental logging api still only behind feature flag
  * `opentelemetry-sdk 1.5.0`
    * Contains stable implementation of metrics, trace, baggage, resource,
      context modules
    * experimental logging implementation still only behind feature flag
* v1.10.0 release (with logging)
  * `opentelemetry-api 1.10.0`
    * Contains stable api's of logging, metrics, trace, baggage, resource,
      context modules
  * `opentelemetry-sdk 1.10.0`
    * Contains stable sdk of logging, metrics, trace, baggage, resource, context
      modules

### Before moving to version 1.0.0

* Major version zero (0.y.z) is for initial development. Anything MAY change at
  any time. The public API SHOULD NOT be considered stable.
