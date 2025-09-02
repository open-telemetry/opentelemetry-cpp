# Export Unsampled Spans Feature

The OpenTelemetry C++ SDK supports an opt-in feature to export unsampled but recording spans from trace processors. This allows collectors to see the full request volume, which is useful for calculating accurate metrics and performing tail-based sampling.

**⚠️ IMPORTANT**: This feature intentionally violates the OpenTelemetry specification when enabled. According to the OpenTelemetry specification, RECORD_ONLY spans should be processed by SpanProcessors but should NEVER be sent to SpanExporters. Use this feature only when you specifically need this behavior and understand the specification implications.

## Overview

By default, the C++ SDK maintains OpenTelemetry specification compliance by only exporting sampled spans (`Decision::RECORD_AND_SAMPLE`). With this feature enabled, spans with `Decision::RECORD_ONLY` (unsampled but recording) will also be exported, which violates the specification.

## Usage

### BatchSpanProcessor

```cpp
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"

// Configure BatchSpanProcessor to export unsampled spans
opentelemetry::sdk::trace::BatchSpanProcessorOptions options;
options.export_unsampled_spans = true;  // Default is false

auto processor = std::make_unique<opentelemetry::sdk::trace::BatchSpanProcessor>(
    std::move(exporter), options);
```

### SimpleSpanProcessor

```cpp
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/simple_processor_options.h"

// Configure SimpleSpanProcessor to export unsampled spans
opentelemetry::sdk::trace::SimpleSpanProcessorOptions options;
options.export_unsampled_spans = true;  // Default is false

auto processor = std::make_unique<opentelemetry::sdk::trace::SimpleSpanProcessor>(
    std::move(exporter), options);
```

## Behavior

When `export_unsampled_spans` is:

- **false** (default): Only sampled spans are exported
- **true**: Both sampled and unsampled recording spans are exported

## Sampling Decisions

The feature respects the sampling decisions made during span creation:

- `Decision::DROP`: Spans are not recorded and never reach the processor (not affected)
- `Decision::RECORD_ONLY`: Spans are recorded but not sampled by default (affected by this feature)
- `Decision::RECORD_AND_SAMPLE`: Spans are recorded and sampled (always exported)

## Backward Compatibility

This feature maintains full backward compatibility:

- Default behavior is unchanged (only sampled spans are exported)
- Existing constructors continue to work as before
- Test spans with invalid contexts are always exported for compatibility

## Example

See [examples/unsampled_spans_demo.cc](../examples/unsampled_spans_demo.cc) for a complete working example demonstrating the feature.