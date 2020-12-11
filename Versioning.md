# Versioning

This document describes the versioning policy for this repository.

## Goals

### API Stability

Once the API for a given signal (spans, logs, metrics, baggage) has been officially released, that API module will function with any SDK that has the same major version, and equal or greater minor or patch version.

For example, libraries that are instrumented with `opentelemetry 1.0.1` will
function in applications using `opentelemetry 1.11.33` or `opentelemetry
1.3.4`.

### SDK Stability

Public portions of the SDK (constructors, configuration, end-user interfaces)
must remain backwards compatible. Internal types are allowed to break.

## Policy

* Releases will follow [SemVer](https://semver.org/).
* Only single source package containing api and sdk for all signals would be released as part of the new GitHub release.
* New telemetry signals will be introduced behind experimental feature flag usng C macro define.
  ```
    #ifdef METRICS_PREVIEW
         <metrics api/sdk definitions>
    #endif
  ```

  As we deliver package in souce form, and the user is responsible to build them for their platform, they have to be
  aware of these feature flags (documented in CHANGELOG.md), and enable it explicitly through their build system (cmake
  / bazel or others ) to use the preview feature.

* New signals will be stabilized via a **minor version bump**, and are not allowed to break existing stable interfaces.
Feature flag would be removed once we have a stable implementation for the signal.

* GitHub releases will be made for all released versions.

## Example Versioning Lifecycle

Purely for illustration purposes, not intended to represent actual releases:

- v0.0.1 release:
   - `opentelemetry 0.0.1`
     - Contains experimental impls of trace, resouce ( no feature flag as major version is 0 )
     - No implementation of logging and metrics available
   - `opentelemetry-contrib 0.0.1`
     - Contains experimental impls of 3rd party trace exporters and propagators ( no feature flag as major version is 0)
     - No implementation for 3rd party logging (and metrics) exporters and propagators
- v1.0.0 release:
   - `opentelemetry 1.0.0`
     - Contains stable impls of trace, baggage and  resource
     - experimental metrics impl behind feature flag
   - `opentelemetry-contrib 1.0.0`
     - Contains stable impls of 3rd party trace exporters and propagators
     - experimental metrics exporters and propagator impls behind feature flag
- v1.5.0 release (with metrics)
   - `opentelemetry 1.5.0`
     - Contains stable impls of metrics, trace, baggage, resource, context modules
     - experimental logging impl still only behind feature flag
   - `opentelemetry-contrib 1.6.0`
     - Contains stable impls of 3rd party trace and metrics exporters and propagators
     - experimental logging exporters and propagator still impls behind feature flag
- v1.10.0 release (with logging)
   - `opentelemetry 1.10.0`
     - Contains stable impls of logging, metrics, trace, baggage, resource, context modules
   - `opentelemetry-contrib 1.12.0`
     - Contains stable impls of 3rd party trace, metrics, and logging exporters and propagators

### Before moving to version 1.0.0:

- 0.0.x : This would be treated as 0.0.major. Things generally expected to work, but anything can break with upgrade.
- 0.x.y : This would be treated as 0.major.minor. There would be distinguish between breaking and non-breaking changes.
- 0.x.y-alpha : Alpha release with most of trace functionality available. Not yet fully tested, and things may break.
- 0.x.y-beta  : full feature trace functionality. Lesser bugs, and release candidate.
