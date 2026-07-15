// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// ~/build/sdk/test/trace/tracer_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-07-15T00:37:18+00:00
// Running /home/devuser/build/sdk/test/trace/tracer_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 3.34, 3.81, 2.96
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// ---------------------------------------------------------------------------------------
// Benchmark                                             Time             CPU   Iterations
// ---------------------------------------------------------------------------------------
// BM_StartSpanTracerDisabled_mean                    5.24 ns         5.24 ns            5
// BM_StartSpanTracerDisabled_median                  5.24 ns         5.24 ns            5
// BM_StartSpanTracerDisabled_stddev                 0.046 ns        0.047 ns            5
// BM_StartSpanTracerDisabled_cv                      0.89 %          0.89 %             5
// BM_StartSpan_mean                                   152 ns          152 ns            5
// BM_StartSpan_median                                 152 ns          152 ns            5
// BM_StartSpan_stddev                                2.74 ns         2.74 ns            5
// BM_StartSpan_cv                                    1.80 %          1.80 %             5
// BM_StartSpanWithScope_mean                          218 ns          218 ns            5
// BM_StartSpanWithScope_median                        217 ns          217 ns            5
// BM_StartSpanWithScope_stddev                       1.75 ns         1.75 ns            5
// BM_StartSpanWithScope_cv                           0.80 %          0.81 %             5
// BM_StartSpanWithImplicitParent_mean                 153 ns          153 ns            5
// BM_StartSpanWithImplicitParent_median               153 ns          153 ns            5
// BM_StartSpanWithImplicitParent_stddev             0.802 ns        0.800 ns            5
// BM_StartSpanWithImplicitParent_cv                  0.52 %          0.52 %             5
// BM_StartSpanWithExplicitParentContext_mean          154 ns          154 ns            5
// BM_StartSpanWithExplicitParentContext_median        154 ns          154 ns            5
// BM_StartSpanWithExplicitParentContext_stddev      0.898 ns        0.899 ns            5
// BM_StartSpanWithExplicitParentContext_cv           0.58 %          0.58 %             5
// BM_StartSpanWithExplicitRootContext_mean            148 ns          148 ns            5
// BM_StartSpanWithExplicitRootContext_median          148 ns          148 ns            5
// BM_StartSpanWithExplicitRootContext_stddev        0.686 ns        0.688 ns            5
// BM_StartSpanWithExplicitRootContext_cv             0.46 %          0.47 %             5
//
// clang-format on

#include <benchmark/benchmark.h>

#include <cstddef>
#include <utility>
#include <vector>

#include "opentelemetry/context/context.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/tracer.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace scope_sdk = opentelemetry::sdk::instrumentationscope;
namespace context   = opentelemetry::context;
using opentelemetry::exporter::memory::InMemorySpanExporter;

namespace
{

constexpr std::size_t kStartSpanBatchSize = 2048;

std::shared_ptr<trace_api::Tracer> CreateTracer(bool is_enabled = true)
{
  auto exporter  = std::make_unique<InMemorySpanExporter>(kStartSpanBatchSize);
  auto processor = std::make_unique<trace_sdk::SimpleSpanProcessor>(std::move(exporter));
  std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto context  = std::make_shared<trace_sdk::TracerContext>(std::move(processors), resource);

  if (!is_enabled)
  {
    auto disabled_config = std::make_unique<scope_sdk::ScopeConfigurator<trace_sdk::TracerConfig>>(
        scope_sdk::ScopeConfigurator<trace_sdk::TracerConfig>::Builder(
            trace_sdk::TracerConfig::Disabled())
            .Build());
    context->SetTracerConfigurator(std::move(disabled_config));
  }
  auto tracer = std::make_shared<trace_sdk::Tracer>(context);

  return tracer;
}

// Test to measure performance for span creation
void BM_StartSpanTracerDisabled(benchmark::State &state)
{
  auto tracer = CreateTracer(false);
  while (state.KeepRunning())
  {
    auto span = tracer->StartSpan("span");
    span->End();
  }
}
BENCHMARK(BM_StartSpanTracerDisabled);

// Test to measure performance for span creation
void BM_StartSpan(benchmark::State &state)
{
  auto tracer = CreateTracer();
  while (state.KeepRunning())
  {
    auto span = tracer->StartSpan("span");
    span->End();
  }
}
BENCHMARK(BM_StartSpan);

// Test to measure performance for single span creation with scope
void BM_StartSpanWithScope(benchmark::State &state)
{
  auto tracer = CreateTracer();
  while (state.KeepRunning())
  {
    auto span = tracer->StartSpan("span");
    trace_api::Scope scope{span};
    span->End();
  }
}
BENCHMARK(BM_StartSpanWithScope);

// Test to measure performance for nested span creation with scope
void BM_StartSpanWithImplicitParent(benchmark::State &state)
{
  auto tracer      = CreateTracer();
  auto parent_span = tracer->StartSpan("parent");
  trace_api::Scope parent_scope{parent_span};
  while (state.KeepRunning())
  {
    auto span = tracer->StartSpan("span");
    span->End();
  }
}

BENCHMARK(BM_StartSpanWithImplicitParent);

// Test to measure performance for nested span creation with manual span context management
void BM_StartSpanWithExplicitParentContext(benchmark::State &state)
{
  auto tracer = CreateTracer();

  auto parent_span = tracer->StartSpan("parent");

  auto init_context   = context::Context{};
  auto parent_context = trace_api::SetSpan(init_context, parent_span);

  while (state.KeepRunning())
  {
    trace_api::StartSpanOptions options;
    options.parent = parent_context;
    auto span      = tracer->StartSpan("span", options);
    span->End();
  }
}
BENCHMARK(BM_StartSpanWithExplicitParentContext);

void BM_StartSpanWithExplicitRootContext(benchmark::State &state)
{
  auto tracer = CreateTracer();

  auto root_context = context::Context{trace_api::kIsRootSpanKey, true};

  while (state.KeepRunning())
  {
    trace_api::StartSpanOptions options;
    options.parent = root_context;
    auto span      = tracer->StartSpan("span", options);
    span->End();
  }
}
BENCHMARK(BM_StartSpanWithExplicitRootContext);

}  // namespace
BENCHMARK_MAIN();
