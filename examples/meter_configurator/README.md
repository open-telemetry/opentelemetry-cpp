# Meter Configurator Example

This example demonstrates how to set a `MeterConfigurator` on construction
of the `MeterProvider` and to update it at runtime using
`MeterProvider::UpdateMeterConfigurator` to enable or disable specific
meters without restarting the application or recreating the meters.

`MeterProvider::UpdateMeterConfigurator` recomputes and applies a new
`MeterConfig` to all existing meters, and the updated configurator is also
used for meters created afterwards. It is safe to call concurrently with
`MeterProvider::GetMeter` and with instrument creation and recording on
existing meters.

Three meters with unique instrumentation scope names are used to simulate
a user application:

- `my_application`: simulated user application
- `my_library`: simulated user library
- `external_library`: simulated external third-party library

The example walks through a simulated cost management and debugging
workflow in four stages:

- Stage 1: Startup. All meters are enabled and all three scopes report
  metrics.
- Stage 2: Steady state. The noisy `external_library` metrics are not
  needed, so its meter is disabled.
- Stage 3: A user reports unexpected behavior. Re-enable the
  `external_library` meter to investigate.
- Stage 4: The investigation completes and the `external_library` meter is
  disabled again.

A `Meter` that is disabled when an instrument is created returns a no-op
instrument, and that instrument stays no-op even if the meter is enabled
later. Create instruments while the meter is enabled, as the classes in
this example do at startup, and then toggle the meters to start and stop
collection.

Disabling a meter stops collection and export for that scope. Measurements
recorded through already-created instruments while the meter is disabled
are still accumulated by the aggregation, so with cumulative temporality
they are included in the total once the meter is enabled again. This is
why `external_library.requests` reports `3` in stage 3 rather than `2`.

## Build and run

```sh
~/build/examples/meter_configurator/example_meter_configurator
```

**Expected output:**

Metrics are exported to stdout via the `OStreamMetricExporter`. The example
uses a long export interval and calls `ForceFlush` at the end of each
stage, so each stage exports exactly once.

```sh
Stage 1: startup, all meters enabled
  my_application, my_library and external_library report metrics
{
  scope name    : my_application
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_application.work_items
  description   : Work items processed by the application
  unit          : {item}
  type          : SumPointData
  value         : 1
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : my_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_library.calls
  description   : Calls into the library
  unit          : {call}
  type          : SumPointData
  value         : 1
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : external_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : external_library.requests
  description   : Requests handled by the external library
  unit          : {request}
  type          : SumPointData
  value         : 1
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}

Stage 2: steady state, external_library meter disabled
  only my_application and my_library report metrics
{
  scope name    : my_application
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_application.work_items
  description   : Work items processed by the application
  unit          : {item}
  type          : SumPointData
  value         : 2
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : my_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_library.calls
  description   : Calls into the library
  unit          : {call}
  type          : SumPointData
  value         : 2
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}

Stage 3: investigating, external_library meter re-enabled
  all three scopes report metrics again
{
  scope name    : my_application
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_application.work_items
  description   : Work items processed by the application
  unit          : {item}
  type          : SumPointData
  value         : 3
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : my_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_library.calls
  description   : Calls into the library
  unit          : {call}
  type          : SumPointData
  value         : 3
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : external_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : external_library.requests
  description   : Requests handled by the external library
  unit          : {request}
  type          : SumPointData
  value         : 3
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}

Stage 4: investigation complete, external_library meter disabled again
  only my_application and my_library report metrics
{
  scope name    : my_application
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_application.work_items
  description   : Work items processed by the application
  unit          : {item}
  type          : SumPointData
  value         : 4
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
{
  scope name    : my_library
  schema url    :
  version       :
  start time    : Mon Jul 27 09:32:35 2026
  end time      : Mon Jul 27 09:32:35 2026
  instrument name       : my_library.calls
  description   : Calls into the library
  unit          : {call}
  type          : SumPointData
  value         : 4
  attributes            :
  resources     :
        service.name: meter_configurator_example
        telemetry.sdk.language: cpp
        telemetry.sdk.name: opentelemetry
        telemetry.sdk.version: 1.29.0-dev
}
```
