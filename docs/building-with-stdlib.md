# Building with Standard C++ Library

Standard Library build flavor works best for statically linking the SDK in a
process (environment where ABI compat is not a requirement), or for
"header-only" implementation of SDK.

Proposed approach cannot be employed for shared libs in environments where
ABI compatibility is required:

- OpenTelemetry SDK binary compiled with compiler A + STL B
- may not be ABI compatible with the main executable compiled with compiler
  C + STL D on a same OS.
Thus, this approach works best only for statically lined / header only use.

## Motivation

In certain scenarios it may be of benefit to compile the OpenTelemetry SDK from
source using standard library container classes (`std::map`, `std::string_view`,
`std::span`, `std::variant`) instead of OpenTelemetry `nostd::` analogs (which
were backported to C++11 and designed as ABI-stable). This PR also can be used
as a foundation for alternate approach - to also allow bindings to [abseil](https://github.com/abseil/abseil-cpp)
classes. That is in environments that would rather prefer `Abseil` classes
over the standard lib or `nostd`.

The approach is to provide totally opaque (from SDK code / SDK developer
perspective) mapping / aliasing from `nostd::` classes back to their `std::`
counterparts, and that is only done in environments where this is possible.

We continue fully supporting both models and run CI for both.

## Pros and Cons

### Using trusted/certified Standard Library

Using standard library implementation classes in customer code and on API
surface instead of `nostd` classes. Certain environments would prefer to have
**standard** instead of non-standard classes due to security, performance,
stability and debug'ability considerations. For example, Visual Studio IDE
provides 1st class Debug experience for Standard containers, plus additional
runtime-checks for Debug builds that use Standard containers.

### Minimizing binary size

No need to marshal types from standard to `nostd`, then back to standard
library means less code involved and less memcpy. We use Standard classes
used elsewhere in the app. We can subsequently improve the process by avoiding
`KeyValueIterable` transform (and, thus, unnecessary memcpy) when we know
that the incoming types does not need to be 'transferred' across ABI boundary.
This is the case for statically linked executables and when SDK is implemented
as 'header-only' library.

### Avoiding unnecessary extra memcpy (perf improvements)

No need to transform from 'native' standard library types, e.g. `std::map` via
`KeyValueIterable` means we can bypass that transformation process, if and when
we know that the ABI compatibility is not a requirement in certain environment.
ETA perf improvement is 1.5%-3% better perf since an extra transform, memcpy,
iteration for-each key-value is avoided.

### ABI stability

Obviously this approach does not work in environment where ABI stability
guarantee must be provided, e.g. for dynamically loadable OpenTelemetry SDK
module and plugins for products such as NGINX, Envoy, etc.

## Scope of changes needed to implement the feature

### Separate flavors of SDK build

Supported build flavors:

- `nostd` - OpenTelemetry backport of classes for C++11. Not using STD Lib.
- `stl`   - Standard Library. Best be compiled with C++20 compaitlbe compiler.
  C++17 may be used with additional dependencies, e.g. MS-GSL or Abseil.
- `absl`  - TODO: this should allow using Abseil C++ lirary only (no MS-GSL).

### Test setup

Tests to validate that all OpenTelemetry functionality is working the same
identical way irrespective of what C++ runtime / STL library it is compiled
with.

### Implementation Details

Feature allows to alias from `nostd::` to `std::` classes for C++17 and above.

Consistent handling of `std::variant` across various OS:

- backport of a few missing variant features, e.g. `std::get` and `std::visit`
  for older version of Mac OS X. Patches that enable proper
  handling of `std::visit` and `std::variant` irrespective of OS version
  to resolve [this quirk](https://stackoverflow.com/questions/52310835/xcode-10-call-to-unavailable-function-stdvisit).

- ability to optionally borrow implementation of C++20 `gsl::span` from
  [Microsoft Guidelines Support Library](https://github.com/microsoft/GSL).
  This is not necessary for C++20 and above compilers.

- ability to use Abseil `absl::variant` for Visual Studio 2015

- set of tools to locally build the SDK with both options across various OS
  and compilers, including vs2015 (C++11), vs2017 (C++17), vs2019 (C++20),
  ubuntu-18.xx (C++17), ubuntu-20.xx/gcc-9 (C++20), Mac OS X (C++17 and above).
