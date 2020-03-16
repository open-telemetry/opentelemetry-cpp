To support scenarios where OpenTelemetry implementations are deployed as binary
plugins, certain restrictions are imposed on portions of the OpenTelemetry API.

Many parts of the standard C++ library don't have well-defined ABIs. To ensure
that a plugin compiled against one version of the standard C++ library can work
with an application or library compiled against a different version of the
standard C++ library, we limit a portion of the OpenTelemetry API to use ABI
stable types.

In the areas of the API where we need ABI stability, we use C++ as an extended
C. We assume that standard language features like inheritance follow a
consistent ABI (vtable layouts, for example, are specified by the
[Itanium ABI](https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vtable)) and
can be used across compilers, but don't rely on the ABI stability of the
standard C++ library classes.

These are some of the rules for where ABI stability is required.

Note: This assumes export maps are used to properly control symbol resolution.

1. Abstract classes in the OpenTelemetry API must use ABI stable types in their
   virtual function signatures. Example

```cpp
class Tracer {
public:
  ...

  virtual void f(const std::string& s); // Bad: std::string doesn't have a well-defined ABI.

  virtual void f(nostd::string_view s); // OK: We provide a ABI stable string_view type.

  void g(const std::vector<int>& v); // OK: g is non-virtual function.
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

2. Singletons defined by the OpenTelemetry API must use ABI stable types since
   they could potentially be shared across multiple instrumented dynamic shared
   objects (DSOs) compiled against different versions of the C++ standard
   library.
