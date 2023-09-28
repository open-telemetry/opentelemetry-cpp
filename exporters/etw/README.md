# Getting Started with OpenTelemetry C++ SDK and ETW exporter on Windows

Event Tracing for Windows (ETW) is an efficient kernel-level tracing facility
that lets you log kernel or application-defined events to a log file. You can
consume the events in real time or from a log file and use them to debug an
application or to determine where performance issues are occurring in the
application.

OpenTelemetry C++ SDK ETW exporter allows the code instrumented using
OpenTelemetry API to forward events to out-of-process ETW listener, for
subsequent data recording or forwarding to alternate pipelines and flows.
Windows Event Tracing infrastructure is available to any vendor or application
being deployed on Windows.

## API support

These are the features planned to be supported by ETW exporter:

- [x] OpenTelemetry Tracing API and SDK headers are **stable** and moving
  towards GA.
- [ ] OpenTelemetry Logging API is work-in-progress, pending implementation of
  [Latest Logging API spec
  here](https://github.com/open-telemetry/oteps/pull/150)
- [ ] OpenTelemetry Metrics API is not implemented yet.

Implementation of OpenTelemetry C++ SDK ETW exporter on Windows OS is `header
only` :

- full definitions of all macros, functions and classes comprising the library
are visible to the compiler in a header file form.
- implementation does not need to be separately compiled, packaged and installed
  in order to be used.

All that is required is to point the compiler at the location of the headers,
and then `#include` the header files into the application source. Compiler's
optimizer can do a much better job when all the library's source code is
available. Several options below may be turned on to optimize the code with the
usage of standard C++ library, Microsoft Guidelines Support library, Google
Abseil Variant library. Or enabling support for non-standard features, such as
8-bit byte arrays support that enables performance-efficient representation of
binary blobs on ETW wire.

## Example project

The following include directories are required, relative to the top-level source
tree of OpenTelemetry C++ repo:

- api/include/
- exporters/etw/include/
- sdk/include/

Code that instantiates ETW TracerProvider, subsequently obtaining a Tracer bound
to `OpenTelemetry-ETW-Provider`, and emitting a span named `MySpan` with
attributes on it, as well as `MyEvent` within that span.

```cpp

#include <map>
#include <string>

#include "opentelemetry/exporters/etw/etw_tracer_exporter.h"

using namespace OPENTELEMETRY_NAMESPACE;
using namespace opentelemetry::exporter::etw;

// Supply unique instrumentation name (ETW Provider Name) here:
std::string providerName = "OpenTelemetry-ETW-Provider";

exporter::etw::TracerProvider tp;

int main(int argc, const char* argv[])
{
  // Obtain a Tracer object for instrumentation name.
  // Each Tracer is associated with unique TraceId.
  auto tracer = tp.GetTracer(providerName, "TLD");

  // Properties is a helper class in ETW namespace that is otherwise compatible
  // with Key-Value Iterable accepted by OpenTelemetry API. Using Properties
  // should enable more efficient data transfer without unnecessary memcpy.

  // Span attributes
  Properties attribs =
  {
    {"attrib1", 1},
    {"attrib2", 2}
  };

  // Start Span with attributes
  auto span = tracer->StartSpan("MySpan", attribs);

  // Emit an event on Span
  std::string eventName = "MyEvent1";
  Properties event =
  {
    {"uint32Key", (uint32_t)1234},
    {"uint64Key", (uint64_t)1234567890},
    {"strKey", "someValue"}
  };
  span->AddEvent(eventName, event);

  // End Span.
  span->End();

  // Close the Tracer on application stop.
  tracer->CloseWithMicroseconds(0);

  return 0;
}
```

Note that different `Tracer` objects may be bound to different ETW destinations.

## Build options and Compiler Defines

While including OpenTelemetry C++ SDK with ETW exporter, the customers are in
complete control of what options they would like to enable for their project
using `Preprocessor Definitions`.

These options affect how "embedded in application" OpenTelemetry C++ SDK code is
compiled:

| Name        | Description                                                      |
|---------------------|------------------------------------------------------------------------------------------------------------------------|
| OPENTELEMETRY_STL_VERSION | Use STL classes for API surface. C++20 is recommended. Some customers may benefit from STL library provided with the compiler instead of using custom OpenTelemetry `nostd::` implementation due to security and performance considerations. |
| HAVE_GSL      | Use [Microsoft GSL](https://github.com/microsoft/GSL) for `gsl::span` implementation. Library must be in include path. Microsoft GSL claims to be the most feature-complete implementation of `std::span`. It may be used instead of `nostd::span` implementation in projects that statically link OpenTelemetry SDK. |
| HAVE_TLD      | Use ETW/TraceLogging Dynamic protocol. This is the default implementation compatible with existing C# "listeners" / "decoders" of ETW events. This option requires an additional optional Microsoft MIT-licensed `TraceLoggingDynamic.h` header. |

## Debugging

### ETW TraceLogging Dynamic Events

ETW supports a mode that is called "Dynamic Manifest", where event may contain
strongly-typed key-value pairs, with primitive types such as `integer`,
`double`, `string`, etc. This mode requires `TraceLoggingDynamic.h` header.
Please refer to upstream repository containining [Microsoft TraceLogging Dynamic
framework](https://github.com/microsoft/tracelogging-dynamic-windows) licensed
under [MIT
License](https://github.com/microsoft/tracelogging-dynamic-windows/blob/main/LICENSE).

Complete [list of ETW
types](https://docs.microsoft.com/en-us/windows/win32/wes/eventmanifestschema-outputtype-complextype#remarks).

OpenTelemetry C++ ETW exporter implements the following type mapping:

| OpenTelemetry C++ API type | ETW type        |
|----------------------------|-----------------|
| bool                       | xs:byte         |
| int (32-bit)               | xs:int          |
| int (64-bit)               | xs:long         |
| uint (32-bit)              | xs:unsignedInt  |
| uint (64-bit)              | xs:unsignedLong |
| double                     | xs:double       |
| string                     | win:Utf8        |

Support for arrays of primitive types is not implemented yet.

Visual Studio 2019 allows to use `View -> Other Windows -> Diagnostic Events` to
capture events that are emitted by instrumented application and sent to ETW
provider in a live view. Instrumentation name passed to `GetTracer` API above
corresponds to `ETW Provider Name`. If Instrumentation name contains a GUID -
starts with a curly brace, e.g. `{deadbeef-fade-dead-c0de-cafebabefeed}` then
the parameter is assumed to be `ETW Provider GUID`.

Click on `Settings` and add the provider to monitor either by its Name or by
GUID. In above example, the provider name is `OpenTelemetry-ETW-Provider`.
Please refer to Diagnostic Events usage instructions
[here](https://docs.microsoft.com/en-us/azure/service-fabric/service-fabric-diagnostics-how-to-monitor-and-diagnose-services-locally#view-service-fabric-system-events-in-visual-studio)
to learn more. Note that running ETW Listener in Visual Studio requires
Elevation, i.e. Visual Studio would prompt you to confirm that you accept to run
the ETW Listener process as Administrator. This is a limitation of ETW
Listeners, they must be run as privileged process.

### ETW events encoded in MessagePack

OpenTelemetry ETW exporter optionally allows to encode the incoming event
payload using [MessagePack](https://msgpack.org/index.html) compact binary
protocol. ETW/MsgPack encoding requires
[nlohmann/json](https://github.com/nlohmann/json) library to be included in the
build of OpenTelemetry ETW exporter. Any recent version of `nlohmann/json` is
compatible with ETW exporter. For example, the version included in
`third_party/nlohmann-json` directory may be used.

There is currently **no built-in decoder available** for this format. However,
there is ongoing effort to include the ETW/MsgPack decoder in
[Azure/diagnostics-eventflow](https://github.com/Azure/diagnostics-eventflow)
project, which may be used as a side-car listener to forward incoming
ETW/MsgPack events to many other destinations, such as:

- StdOutput (console output)
- HTTP (json via http)
- Application Insights
- Azure EventHub
- Elasticsearch
- Azure Monitor Logs

And community-contributed exporters:

- Google Big Query output
- SQL Server output
- ReflectInsight output
- Splunk output

[This PR](https://github.com/Azure/diagnostics-eventflow/pull/382) implements
the `Input adapter` for OpenTelemetry ETW/MsgPack protocol encoded events for
Azure EventFlow.

Other standard tools for processing ETW events on Windows OS, such as:

- [PerfView](https://github.com/microsoft/perfview)
- [PerfViewJS](https://github.com/microsoft/perfview/tree/main/src/PerfViewJS)

will be augmented in future with support for ETW/MsgPack encoding.

## Addendum

This document needs to be supplemented with additional information:

- [ ] mapping between OpenTelemetry fields and concepts and their corresponding
  ETW counterparts
- [ ] links to E2E instrumentation example and ETW listener
- [ ] Logging API example
- [ ] Metrics API example (once Metrics spec is finalized)
- [ ] example how ETW Listener may employ OpenTelemetry .NET SDK to 1-1
  transform from ETW events back to OpenTelemetry flow
- [ ] links to NuGet package that contains the source code of SDK that includes
  OpenTelemetry SDK and ETW exporter
