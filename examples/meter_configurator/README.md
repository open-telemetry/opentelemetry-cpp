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

Instruments observe the enabled state of the meter that created them, so
they do not need to be recreated after a configurator update. An instrument
created while its meter is disabled starts recording once the meter is
enabled, and stops again when the meter is disabled. This matters for
third-party instrumentation, which an application cannot force to rebuild
its instruments.

Disabling a meter stops collection and export for that scope, and
measurements recorded while it is disabled are dropped rather than buffered.
They are not revealed by a later export, which is why
`external_library.requests` reports `2` in stage 3: the stage 2 measurement
is not retained.

## Build and run

```sh
~/build/examples/meter_configurator/example_meter_configurator
```

**Expected output:**

Metrics are exported to stdout via the `OStreamMetricExporter`. The example
calls `ForceFlush` at the end of each stage. The periodic reader also
collects once when it starts, so the exact number and ordering of exported
batches varies between runs; the transcript below shows one run and is not
an exact expected output.

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
  value         : 2
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
