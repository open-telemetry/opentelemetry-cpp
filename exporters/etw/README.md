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

## Tracer lifetime and reuse

For ETW, the tracer name maps to the ETW provider name/GUID. Applications
should create one tracer per provider name (and encoding) and reuse it for the
process or component lifetime. `TracerProvider::GetTracer()` may cache tracers
internally, so avoid calling `GetTracer()->StartSpan()` in hot paths. Instead,
obtain the tracer once during setup and reuse it for span creation.

It is recommended to consume this exporter via
[vcpkg](https://vcpkg.io/en/package/opentelemetry-cpp).
