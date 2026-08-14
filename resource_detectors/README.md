# Resource Detectors

Resource detectors populate the OpenTelemetry `Resource` with attributes
describing the environment where the application is running.

Refer to the linked semconv pages for the stability status of each detector
and its attributes.

## Available detectors

### Container Resource Detector

[Semconv: Container](https://opentelemetry.io/docs/specs/semconv/resource/container/)

Reads the container ID from the cgroup file (e.g. Docker, Kubernetes).

| Attribute | Description |
| --- | --- |
| `container.id` | Container ID (e.g. `a3bf90e006b2`) |

### Host Resource Detector

[Semconv: Host](https://opentelemetry.io/docs/specs/semconv/resource/host/)

Reads host information from the operating system.

| Attribute | Description |
| --- | --- |
| `host.name` | Hostname (from `gethostname` / `uname`) |
| `host.arch` | CPU architecture (e.g. `amd64`, `arm64`) |
| `host.id` | Machine ID (from `/etc/machine-id` on Linux, registry on Windows) |

### Process Resource Detector

[Semconv: Process](https://opentelemetry.io/docs/specs/semconv/resource/process/)

Reads process information from the operating system.

| Attribute | Description |
| --- | --- |
| `process.pid` | Process ID |
| `process.executable.path` | Full path to the process executable |

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
`Registry`, making it available when the SDK is initialised from a config file.
The detector is a private dependency of the builder and is not exposed
transitively — link only the builder targets you need.

Detectors can be declared under the `resource.detection/development` entry in
the configuration YAML file.

Example configuration:

```yaml
resource:
  detection/development:
    detectors:
      - container:
      - process:
      # NOTE: host and service detectors cannot be configured currently
      - host:    # needs a builder (`Registry::SetHostResourceDetectorBuilder`)
      - service: # not implemented (github.com/open-telemetry/opentelemetry-cpp/issues/4414)
```

Call `Register` for each builder before creating the SDK:

```cpp
#include "opentelemetry/resource_detectors/container_detector_builder.h"
#include "opentelemetry/resource_detectors/process_detector_builder.h"

opentelemetry::resource_detector::ContainerDetectorBuilder::Register(registry.get());
opentelemetry::resource_detector::ProcessDetectorBuilder::Register(registry.get());
```

## Linking with CMake

### Detector targets: Link to the detector for direct use in an application

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

### Builder targets: Link to builders for declaritive configuration

Builder targets make detectors available for declaritive configuration and hide
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
| `opentelemetry-cpp::container_resource_detector_builder` | Container detector builder for declaritive configuration |
| `opentelemetry-cpp::env_entity_resource_detector` | Env entity detector |
| `opentelemetry-cpp::host_resource_detector` | Host detector |
| `opentelemetry-cpp::process_resource_detector` | Process detector |
| `opentelemetry-cpp::process_resource_detector_builder` | Process detector builder for declaritive configuration |

## Linking with Bazel

### Detector targets: Link detector for direct use in an application

Link to all available detectors:

```python
deps = ["//resource_detectors"]
```

Link to only the detectors you use:

```python
deps = ["//resource_detectors:host_resource_detector"]
```

### Builder targets: Link builders for declaritive configuration

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
| `//resource_detectors:process_resource_detector` | Process detector |
| `//resource_detectors:process_resource_detector_builder` | Process detector builder |
