# Semantic Conventions

## Tooling

The following files

* [trace/semantic_conventions.h](/api/include/opentelemetry/trace/semantic_conventions.h)
* [sdk/resource/semantic_conventions.h](/sdk/include/opentelemetry/sdk/resource/semantic_conventions.h)

are generated automatically.

The source data is in YAML format, located in the
[specification](https://github.com/open-telemetry/opentelemetry-specification/blob/main/semantic_conventions/README.md).

A code [generator](https://github.com/open-telemetry/build-tools/blob/main/semantic-conventions/README.md)
parses the YAML data from the specs,
and generate code for various languages using a template engine.

For opentelemetry-cpp, the templates are located in
[buildscripts](/buildscripts/semantic-convention/templates/SemanticAttributes.h.j2).

A [generate.sh](/buildscripts/semantic-convention/generate.sh) script
downloads the specs, invokes the generator using Docker,
and generates code for opentelemetry-cpp.

## Instructions

### Find latest specifications

Check for the latest
[specification](https://github.com/open-telemetry/opentelemetry-specification/releases)
release, and note the release tag number.

For example, tag v1.12.0

### Use latest specifications

Set the `SEMCONV_VERSION` number in the
[generate.sh](/buildscripts/semantic-convention/generate.sh) script.

For example,

```shell
SEMCONV_VERSION=1.12.0
```

### Generate code

Run the generate.sh script.

Inspect the generated files,
to verify they were updated (check the version number in SCHEMA_URL).

For example,

```cpp
static constexpr const char *SCHEMA_URL = "https://opentelemetry.io/schemas/1.12.0";
```

### Format code

Apply clang-format.

### Update CHANGELOG

Add a `CHANGELOG` entry for the semantic conventions.

For example,

```md
* [SEMANTIC CONVENTIONS] Upgrade to version 1.12.0
```

### Commit

Commit and file a pull request.
