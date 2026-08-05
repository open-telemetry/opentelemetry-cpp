# Logger Configurator Example

This example demonstrates how to set a `LoggerConfigurator` on construction
of the `LoggerProvider` and to update it at runtime using
`LoggerProvider::UpdateLoggerConfigurator` to control the minimum severity
and enabled state of specific loggers without restarting the application or
recreating the loggers.

`LoggerProvider::UpdateLoggerConfigurator` is thread-safe and updates the
`LoggerConfig` on all existing loggers.

Three loggers with unique instrumentation scope names are used to simulate
a user application:

- `my_application`: simulated user application
- `my_library`: simulated user library
- `external_library`: simulated external third-party library

The example walks through a simulated debugging workflow in four stages:

- Stage 1: Production baseline. `my_application` and `my_library` loggers
  with Warn minmum severity and the
  `external_library` logger is disabled.
- Stage 2: User reports unexpected behavior. Lower `my_application` and
  `my_library` loggers to Debug and `external_library` logger remains disabled.
- Stage 3: Debug logs implicate `external_library`. Enable its logger at Debug and
  Raise `my_application` back to Warn.
- Stage 4: Root cause found and fixed. Restore the production baseline.

## Build and run

```sh
~/build/examples/logger_configurator/example_logger_configurator
```

**Expected output:**

Log records are exported to stdout via the `OStreamLogRecordExporter`.

```sh
Stage 1: production baseline
  my_application and my_library at Warn. external_library disabled
{
  timestamp          : 0
  observed_timestamp : 1783528026109982345
  severity_num       : 13
  severity_text      : WARN
  body               : MyApplication: my_library invoke failed with an error
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_application
    version          :
    schema_url       :
    attributes       :
}

Stage 2: investigating, loggers lowered to Debug
  my_application and my_library at Debug. external_library disabled
{
  timestamp          : 0
  observed_timestamp : 1783528026110271494
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyApplication: Execute called...
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_application
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026110499958
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: Execute called...
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026110864190
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: external_library returned an error
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026111027097
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: Execute complete
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026111207651
  severity_num       : 13
  severity_text      : WARN
  body               : MyApplication: my_library invoke failed with an error
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_application
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026111366894
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyApplication: Execute complete
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_application
    version          :
    schema_url       :
    attributes       :
}

Stage 3: narrowing down, my_library and external_library at Debug
  my_library and external_library at Debug. my_application at Warn
{
  timestamp          : 0
  observed_timestamp : 1783528026111618733
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: Execute called...
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026111778229
  severity_num       : 5
  severity_text      : DEBUG
  body               : ExternalModule: Execute called...
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : external_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112008544
  severity_num       : 5
  severity_text      : DEBUG
  body               : ExternalModule: executing...
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : external_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112254877
  severity_num       : 17
  severity_text      : ERROR
  body               : ExternalModule: error detected!
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : external_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112412571
  severity_num       : 5
  severity_text      : DEBUG
  body               : ExternalModule: some error details.
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : external_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112638912
  severity_num       : 5
  severity_text      : DEBUG
  body               : ExternalModule: Execute complete
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : external_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112799800
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: external_library returned an error
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112858234
  severity_num       : 5
  severity_text      : DEBUG
  body               : MyModule: Execute complete
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_library
    version          :
    schema_url       :
    attributes       :
}
{
  timestamp          : 0
  observed_timestamp : 1783528026112886063
  severity_num       : 13
  severity_text      : WARN
  body               : MyApplication: my_library invoke failed with an error
  resource           :
    telemetry.sdk.version: 1.28.0-dev
    telemetry.sdk.name: opentelemetry
    telemetry.sdk.language: cpp
    service.name: logger_configurator_example
  attributes         :
  event_id           : 0
  event_name         :
  trace_id           : 00000000000000000000000000000000
  span_id            : 0000000000000000
  trace_flags        : 00
  scope              :
    name             : my_application
    version          :
    schema_url       :
    attributes       :
}

Stage 4: external_library error resolved, production baseline restored
  my_application and my_library at Warn. external_library disabled
```
