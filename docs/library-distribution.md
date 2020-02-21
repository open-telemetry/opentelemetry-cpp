# Library Distribution

This document describes the considerations and recommendations for using the
OpenTelemetry-cpp library.

## Definitions

### Binary vs Source Distribution

The OpenTelemetry-cpp libraries (API and SDK) and any library that depends on
them are expected to be distributed as either "source" or "binary" artefacts.
This document defines these distribution models as:

- **Source** - The source code is shipped as-is and can be built as desired
- **Binary** - The library and/or its dependants may be distributed as pre-built
binaries which can be built using different toolchains or compilers.

This document will focus on the specific challenges of the "binary" distribution
model.

### Library Binding Model

The library and its dependants can be distributed and bound (loaded) in multiple
ways which are defined as follows:

- **Static Linkage** - The library is distributed as a compiled artefact (`.a`
extension on Unix-like systems) which is linked and bound at compile-time.
- **Dynamic Linkage with Early Binding** - The library is distributed as a
compiled artefact (`.so` on Unix-like systems), and its binding is defined at
compile-time. It is loaded by the dynamic linker when the application/library
depending on it is started/loaded.
- **Dynamic Linkage with Late Binding** - The library is distributed as a 
compiled artefact (`.so` on Unix-like systems), only its interface is known at
compile-time. It is loaded dynamically (using `dlopen` on POSIX).

*NOTE: A library may itself link in other libraries based on any of the above.*

### Components

The following components will be considered for analysing the impact of
different binding models and the recommendations.

- **OpenTelemetry Libraries** - The libraries produced from this project,
the API library will contain some globals/singletons such as `TracerProvider`.
- **OpenTelemetry Instrumented Libraries** - A library that depends on the
OpenTelemetry API library to generate telemetry.
- **OpenTelemetry Implementations or Exporters** - A specific implementation of
the OpenTelemetry API or the `exporters` may be provided externally (e.g.
distributed by a vendor).
- **Executable Binary** - The compiled binary that has `main()`, this also
includes interpreters such as Python which will be considered. This binary may
itself instrument using OpenTelemetry or register a specific implementation.

For more information on language-agnostic library, please see:
[OpenTelemetry Specification Library Guidelines](https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/library-guidelines.md).

### ABI Compatibility

Some guarantees regarding binary compatibility will be necessary given the
binary distribution model supported by this project. The policies and approach
regarding this will be documented in [ABI Policy](abi-policy.md).

## Recommendations

### 1. Avoid multiple singletons

Multiple singletons could be linked in and available at run-time. For example,
if an instrumented library statically links in the OpenTelemetry API, and then
the application that loads this library statically links in its own. The
consequences of this would be that even if the application registers a specific
implementation/exporter using a singleton, the library will not be able to use
this, and will need to somehow have its own singleton set.

- **OpenTelemetry Libraries**: Can be distributed for linkage with any of the
three models.

- **OpenTelemetry Instrumented Libraries**: Can be distributed for linkage with
any of the three models but SHOULD NOT statically link the OpenTelemetry API.

- **OpenTelemetry Implementations or Exporters**: Can be distributed for linkage
with any of the three models but SHOULD NOT statically link the OpenTelemetry
API.

## Example Scenarios

### Statically compiled binary executable

Executable binaries can be distributed with static linkage for all libraries
known at compile time. In this case the OpenTelemetry API Library can be linked
in statically, guaranteeing that only a single symbol is exported for
singletons.

### Dynamically linked binary executable

TBD: As above but with dynamic linkage

### Non OpenTelemetry aware application with OpenTelemetry capability library

TBD: Nginx Case
