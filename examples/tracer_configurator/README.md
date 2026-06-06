# Tracer Configurator Example

This example demonstrates how to set a `TracerConfigurator` on construction of the `TracerProvider` and to update it using `TracerProvider::UpdateTracerConfigurator`
to enable or disable specific tracers at runtime without restarting the application or recreating the tracers.

`TracerProvider::UpdateTracerConfigurator` is thread-safe and updates the `TracerConfig` on all
existing tracers.

Three tracers with unique instrumentation scope names are used to simulate a user application:

- `my_application`: simulated user application
- `my_library`: simulated user library
- `external_library`: simulated external third-party library

The example walks through a simulated debugging workflow in four stages:

Stage 1: Disable all tracers by default
Stage 2: Enable `my_application` and `my_library` tracers to identify the failed spans in user code
Stage 3: Enable all tracers to observe the root cause failure in the `external_library` span
Stage 4: Disable all tracers after investigation completes

## Build and run

```sh
~/build/examples/tracer_configurator/example_tracer_configurator
```

**Expected output:**

Spans are exported to stdout via the `OStreamSpanExporter`.

```sh
=== Stage 1: all tracers initially disabled ===
[my_application] Execute
[my_library] Execute
[external_library] Execute

=== Stage 2: enable only 'my_application' and 'my_library' tracers ===
[my_application] Execute
[my_library] Execute
[external_library] Execute
{
  name          : MyModule.Execute
  trace_id      : 114712fa76ad1cf9ad4579c2b9518306
  span_id       : 9d153f90c7d7f340
  tracestate    :
  parent_span_id: fdc14dfb5123734b
  start         : 1780785207912694667
  duration      : 15368223
  description   :
  span kind     : Internal
  status        : Error
  attributes    :
  events        :
  links         :
  resources     :
        telemetry.sdk.version: 1.28.0-dev
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.language: cpp
        service.name: tracer_configurator_example
  instr-lib     : my_library
}
{
  name          : MyApplication.Execute
  trace_id      : 114712fa76ad1cf9ad4579c2b9518306
  span_id       : fdc14dfb5123734b
  tracestate    :
  parent_span_id: 0000000000000000
  start         : 1780785207912633155
  duration      : 15497061
  description   :
  span kind     : Internal
  status        : Error
  attributes    :
  events        :
  links         :
  resources     :
        telemetry.sdk.version: 1.28.0-dev
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.language: cpp
        service.name: tracer_configurator_example
  instr-lib     : my_application
}

=== Stage 3: enable all tracers ===
[my_application] Execute
[my_library] Execute
[external_library] Execute
{
  name          : ExternalModule.Execute
  trace_id      : 0531e8205c197402d4a0a3676cd62590
  span_id       : 80e266e7bbaf8779
  tracestate    :
  parent_span_id: bdb35ecc3959a13f
  start         : 1780785207933329412
  duration      : 10081972
  description   : Something went wrong in external_library
  span kind     : Internal
  status        : Error
  attributes    :
  events        :
  links         :
  resources     :
        telemetry.sdk.version: 1.28.0-dev
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.language: cpp
        service.name: tracer_configurator_example
  instr-lib     : external_library
}
{
  name          : MyModule.Execute
  trace_id      : 0531e8205c197402d4a0a3676cd62590
  span_id       : bdb35ecc3959a13f
  tracestate    :
  parent_span_id: b14ce47b2d3e9057
  start         : 1780785207928203132
  duration      : 15260338
  description   :
  span kind     : Internal
  status        : Error
  attributes    :
  events        :
  links         :
  resources     :
        telemetry.sdk.version: 1.28.0-dev
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.language: cpp
        service.name: tracer_configurator_example
  instr-lib     : my_library
}
{
  name          : MyApplication.Execute
  trace_id      : 0531e8205c197402d4a0a3676cd62590
  span_id       : b14ce47b2d3e9057
  tracestate    :
  parent_span_id: 0000000000000000
  start         : 1780785207928189006
  duration      : 15305320
  description   :
  span kind     : Internal
  status        : Error
  attributes    :
  events        :
  links         :
  resources     :
        telemetry.sdk.version: 1.28.0-dev
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.language: cpp
        service.name: tracer_configurator_example
  instr-lib     : my_application
}

=== Stage 4: disable all tracers ===
[my_application] Execute
[my_library] Execute
[external_library] Execute
```
