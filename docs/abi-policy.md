# Application Binary Interface (ABI) Policy

To support scenarios where opentelemetry-cpp is included in multiple libraries
that are combined into a single application (for example as binary plugins, or
as independently instrumented libraries), certain restrictions are imposed on
portions of the OpenTelemetry API to ensure ABI stability and predictable
behavior. See [library distribution](./library-distribution.md) for how
components control their exported symbols in that scenario.

Many parts of the C++ standard library don't have well-defined ABIs. To ensure
that a component compiled against one version of the C++ standard library can
work with an application or library compiled against a different version of the
C++ standard library, we limit a portion of the OpenTelemetry API to use ABI
stable `nostd::` types by default. The `OPENTELEMETRY_STL_VERSION` preprocessor macro
(defined by the CMake `WITH_STL` build option) can be set to replace
these ABI stable types with their `std::` equivalents (see [building with the
standard C++ library](./building-with-stdlib.md) for when this is appropriate).

In the areas of the API where we need ABI stability, we use C++ as an extended
C. We assume that standard language features like inheritance follow a
consistent ABI (vtable layouts, for example, are specified by the [Itanium
ABI](https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vtable)) and can be used
across compilers, but don't rely on the ABI stability of the C++ standard
library classes.

ABI stability is provided for the OTel API and is not required for
OTel SDK-provided interfaces (e.g., resource detectors, exporters, processors, samplers).

Within a stable ABI version, non-breaking additive changes may be accepted, for example:

- add new non-virtual member functions to an existing API class
- add new overloads of an existing non-virtual function
- add new types
- append new values to an enum

The following changes are considered breaking and must target an
experimental ABI version:

- use of non-ABI-stable types in virtual function signatures or
  singletons of an API class
- change the order, add, remove, or modify the signature of virtual
  functions on an API class
- change the memory layout of a type that crosses the ABI boundary
- change the definition or observable behavior of an inline function,
  template, or `inline`/`constexpr` variable in an API header

The following sections describe common cases where ABI stability is broken
and provide examples of how to apply breaking changes.

For the ABI-stable OTel API, breaking changes must target the
experimental ABI version, guarded by the `OPENTELEMETRY_ABI_VERSION_NO`
preprocessor macro (shown by the `#if
OPENTELEMETRY_ABI_VERSION_NO >= 2` guards in the examples). See the [ABI version
policy](./abi-version-policy.md) for which ABI versions are stable versus
experimental.

## Virtual function signatures

Abstract classes in the OpenTelemetry API must use ABI stable types in their
virtual function signatures.

Example:

```cpp
class Tracer {
public:
  ...

  // Bad: std::string doesn't have a well-defined ABI.
  virtual void f(const std::string& s);

  // OK: We provide a ABI stable string_view type.
  virtual void f(nostd::string_view s);

  // OK: g is non-virtual function.
  void g(const std::vector<int>& v);
  ...
};
```

The ABI restrictions don't apply to concrete classes.

```cpp
class TracerImpl final : public Tracer {
public:
  // ...
private:
  std::string s; // OK
};
```

Singletons defined by the OpenTelemetry API must use ABI stable types since they
could potentially be shared across multiple instrumented dynamic shared objects
(DSOs) compiled against different versions of the C++ standard library.

## Virtual function tables

The set, order, and signatures of virtual functions
on an existing API class must not change within a stable ABI version. Doing so
alters the vtable layout and breaks
binary compatibility for callers built against the previous layout. New or
changed virtual functions must instead be introduced in an experimental ABI
version.

Example:

```cpp
class Logger {
public:
  virtual ~Logger() = default;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  // OK: adding a new virtual method under the experimental ABI guard, so the stable ABI (v1) vtable layout is unchanged.
  virtual void NewVirtualMethod() noexcept {}
#endif
};
```

## Memory layout

Within a stable ABI version, the memory layout of any type that crosses the ABI
boundary must not change.

This applies to the abstract API classes, the ABI stable value types (such as
`nostd::string_view` and `nostd::span`), and singletons. Any change that alters
layout, such as the order, type, or alignment of non-static data members, or the
set and order of base classes, must be delivered in an experimental ABI version.
The same applies to enumerations: do not change the underlying type or renumber
existing enumerators.

Example:

```cpp
class Logger
{
public:
  // OK: adding a non-virtual method does not change the memory layout.
  bool NewMethod() const noexcept;

private:

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  // OK: new data member added only under the experimental ABI guard.
  bool new_member_{};
#endif
};
```

## Definitions in API headers

Entities defined in ABI-stable API headers (inline functions, templates, and
`inline`/`constexpr` variables) must not change their definition or observable
behavior within a stable ABI version.

A single application may link multiple independently built libraries that were
compiled against different opentelemetry-cpp API header versions, or with
different API-level compiler definitions. Each carries its own definition of
these inline entities. The C++ [One Definition
Rule](https://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule)
requires every definition to be identical. If they differ, the linker silently
keeps one unspecified definition. A library can then run a definition it was not
built against, producing unexpected behavior.

Any change to API headers that impacts observable behavior must instead be
delivered in an experimental ABI version. Critical correctness or security fixes
may be exceptions. All such changes must be noted in
`CHANGELOG.md`.

Example:

`Logger::EmitLogRecord(args...)` applies the severity filter on log
records (and may drop the record). This is an observable behavior change and
only allowed under the experimental ABI (v2).

```cpp
class Logger
{
public:
  template <class... ArgumentType>
  void EmitLogRecord(ArgumentType &&...args)
  {
  #if OPENTELEMETRY_ABI_VERSION_NO >= 2
    // Behavior change added to the experimental ABI
    const Severity arg_severity = detail::FindSeverityInArgs(args...);
    if (arg_severity != Severity::kInvalid && !Enabled(arg_severity))
    {
      return;
    }
    nostd::unique_ptr<LogRecord> log_record = CreateLogRecord(/* context_or_span */);
  #else
    // Stable ABI behavior
    nostd::unique_ptr<LogRecord> log_record = CreateLogRecord();
  #endif

    ...
  }
};
```
