# Resource Detectors

Resource detectors populate `Resource Entity` attributes
describing the environment where the application is running.

Detectors in this project will be limited to those defined as `built-in` by the
OTel [Resource SDK Spec](https://opentelemetry.io/docs/specs/otel/resource/sdk/#resource-detector-name)
and [configuration schema](https://github.com/open-telemetry/opentelemetry-configuration/blob/main/schema/resource.yaml)
along with the experimental
[EnvEntity Detector](https://opentelemetry.io/docs/specs/otel/entities/entity-propagation/#enventitydetector).

Currently the `built-in` detectors include:

- `service`
- `container`
- `host`
- `process`

The detectors should support all tested platforms (Linux, macOS, and Windows)
and must follow the semantic conventions for
[Entities](https://opentelemetry.io/docs/specs/semconv/registry/entities/).

## Built-in detectors

Each detector section lists currently supported Entity attributes by platform.
Contributions to detect more attributes for each Entity are welcome.

### Service Resource Detector

[Entity: Service](https://opentelemetry.io/docs/specs/semconv/registry/entities/service/)

| Attribute | Description | Linux | macOS | Windows |
| --- | --- | --- | --- | --- |
| `service.name` | From `OTEL_SERVICE_NAME` when set | Yes | Yes | Yes |
| `service.name` | Fallback `unknown_service:<process.executable.name>` or `unknown_service` | Yes | Yes | Yes |
| `service.instance.id` | Stable RFC 4122 UUID version 4 for the current process | Yes | Yes | Yes |

`service.name` is read only from the `OTEL_SERVICE_NAME` environment variable. When
that variable is unset, the detector falls back to
`unknown_service:<process.executable.name>` when the executable name is
available for the current process, otherwise `unknown_service`.

`service.instance.id` is generated once per process and remains stable until the
process ID changes (for example after `fork()`).

Limitations:

- The detector does not read `OTEL_RESOURCE_ATTRIBUTES`.
- Executable-name fallback reuses process detector utilities; on macOS the
  executable name is available for the current process only.

### Container Resource Detector

[Entity: Container](https://opentelemetry.io/docs/specs/semconv/registry/entities/container/)

| Attribute | Description | Linux | macOS | Windows |
| --- | --- | --- | --- | --- |
| `container.id` | Container ID from `/proc/self/cgroup` | Yes | No | No |

Limitation: this detector depends on Linux cgroup data and may return no value
outside containers or when cgroup data is unavailable.

### Host Resource Detector

[Entity: Host](https://opentelemetry.io/docs/specs/semconv/registry/entities/host/)

| Attribute | Description | Linux | macOS | Windows |
| --- | --- | --- | --- | --- |
| `host.name` | Hostname from OS APIs | Yes | Yes | Yes |
| `host.arch` | Architecture mapped at build time | Yes | Yes | Yes |
| `host.id` | Host ID from OS-specific sources | Yes | Yes | Yes |

Limitation: `host.id` is omitted when the platform-specific source is missing
or inaccessible.

### Process Resource Detector

[Entity: Process](https://opentelemetry.io/docs/specs/semconv/registry/entities/process/)

| Attribute | Description | Linux | macOS | Windows |
| --- | --- | --- | --- | --- |
| `process.pid` | Process ID | Yes | Yes | Yes |
| `process.executable.path` | Path via `/proc` (Linux) or Win32 APIs | Yes | Yes | Yes |
| `process.executable.name` | Basename of the executable path | Yes | Yes | Yes |
| `process.creation.time` | Process start time in ISO 8601 UTC | Yes | Yes | Yes |
| `process.owner` | Username of the process owner | Yes | Yes | Yes |

Limitations:

- On macOS, `process.executable.path` and `process.executable.name`
  are resolved via `_NSGetExecutablePath()`,
  which only works for the **current process**. These attributes are always
  populated for the running process, but cannot be resolved for an arbitrary PID.

### Env Entity Resource Detector (Experimental)

[Spec: Entity Propagation — `OTEL_ENTITIES`](https://opentelemetry.io/docs/specs/otel/entities/entity-propagation/)
(experimental)

Reads entity definitions from the `OTEL_ENTITIES` environment variable. Each
entity specifies a type, identifying attributes, and optional descriptive
attributes. The resulting resource attributes depend entirely on what is set in
the environment variable.

Example value:

```bash
OTEL_ENTITIES=service{service.name=my-app,service.instance.id=i-1}[service.version=1.0.0]
```

## Declarative configuration

Each builder registers its detector with the SDK
[declarative configuration](https://opentelemetry.io/docs/specs/otel/configuration/)
`Registry`, making it available when the SDK is initialized using programmatic
configuration or from a yaml file.

Detectors can be declared under the `resource.detection/development` entry in
the configuration YAML file.

Example configuration:

```yaml
resource:
  detection/development:
    detectors:
      - container:
      - host:
      - process:
      - service:
```

Call `Register` for each builder before creating the SDK:

```cpp
#include "opentelemetry/resource_detectors/container_detector_builder.h"
#include "opentelemetry/resource_detectors/host_detector_builder.h"
#include "opentelemetry/resource_detectors/process_detector_builder.h"
#include "opentelemetry/resource_detectors/service_detector_builder.h"

opentelemetry::resource_detector::ContainerDetectorBuilder::Register(registry.get());
opentelemetry::resource_detector::HostDetectorBuilder::Register(registry.get());
opentelemetry::resource_detector::ProcessDetectorBuilder::Register(registry.get());
opentelemetry::resource_detector::ServiceDetectorBuilder::Register(registry.get());
```

## Linking with CMake

### Linking to detectors directly (not required for declarative config)

Link to all available detectors:

```cmake
find_package(opentelemetry-cpp CONFIG REQUIRED COMPONENTS resource_detectors)
target_link_libraries(my_target PRIVATE opentelemetry-cpp::resource_detectors)
```

Link to only the detectors you use:

```cmake
find_package(opentelemetry-cpp CONFIG REQUIRED COMPONENTS resource_detectors)
target_link_libraries(my_target PRIVATE opentelemetry-cpp::host_resource_detector)
```

### Linking to builders for declarative configuration

Builder targets make detectors available for declarative configuration and hide
the concrete detector implementations.

Link all builders:

```cmake
find_package(opentelemetry-cpp CONFIG REQUIRED COMPONENTS resource_detectors)
target_link_libraries(my_target PRIVATE
  opentelemetry-cpp::resource_detectors_builders)
```

Link to only the builders you use:

```cmake
find_package(opentelemetry-cpp CONFIG REQUIRED COMPONENTS resource_detectors)
target_link_libraries(my_target PRIVATE
  opentelemetry-cpp::process_resource_detector_builder)
```

### Available targets

All targets are part of the `resource_detectors` component.

| Target | Description |
| --- | --- |
| `opentelemetry-cpp::resource_detectors` | Interface: links all detectors |
| `opentelemetry-cpp::resource_detectors_builders` | Interface: links all builders |
| `opentelemetry-cpp::container_resource_detector` | Container detector |
| `opentelemetry-cpp::container_resource_detector_builder` | Container detector builder for declarative configuration |
| `opentelemetry-cpp::env_entity_resource_detector` | Env entity detector |
| `opentelemetry-cpp::host_resource_detector` | Host detector |
| `opentelemetry-cpp::host_resource_detector_builder` | Host detector builder for declaritive configuration |
| `opentelemetry-cpp::process_resource_detector` | Process detector |
| `opentelemetry-cpp::process_resource_detector_builder` | Process detector builder for declarative configuration |
| `opentelemetry-cpp::service_resource_detector` | Service detector |
| `opentelemetry-cpp::service_resource_detector_builder` | Service detector builder for declarative configuration |

## Linking with Bazel

### Linking to detectors directly

Link to all available detectors:

```python
deps = ["//resource_detectors"]
```

Link to only the detectors you use:

```python
deps = ["//resource_detectors:host_resource_detector"]
```

### Linking to builders only

Builder targets make detectors available for declarative configuration and hide
the concrete detector implementations.

Link all builders:

```python
deps = ["//resource_detectors:resource_detectors_builders"]
```

Link to only the builders you use:

```python
deps = ["//resource_detectors:process_resource_detector_builder"]
```

### Available targets (Bazel)

| Target | Description |
| --- | --- |
| `//resource_detectors` | Interface: links all detectors |
| `//resource_detectors:resource_detectors_builders` | Interface: links all builders |
| `//resource_detectors:container_resource_detector` | Container detector |
| `//resource_detectors:container_resource_detector_builder` | Container detector builder |
| `//resource_detectors:env_entity_resource_detector` | Env entity detector |
| `//resource_detectors:host_resource_detector` | Host detector |
| `//resource_detectors:host_resource_detector_builder` | Host detector builder |
| `//resource_detectors:process_resource_detector` | Process detector |
| `//resource_detectors:process_resource_detector_builder` | Process detector builder |
| `//resource_detectors:service_resource_detector` | Service detector |
| `//resource_detectors:service_resource_detector_builder` | Service detector builder |
