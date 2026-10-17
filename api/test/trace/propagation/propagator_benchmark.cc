// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off

// ~/build/api/test/trace/propagation/propagator_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-10-13T16:12:49+00:00
// Running /home/devuser/build/api/test/trace/propagation/propagator_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 0.11, 1.06, 1.26
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------
// Benchmark                                                     Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------
// BM_HttpTraceContextInject_mean                             20.1 ns         20.1 ns            5
// BM_HttpTraceContextInject_median                           20.1 ns         20.1 ns            5
// BM_HttpTraceContextInject_stddev                          0.082 ns        0.083 ns            5
// BM_HttpTraceContextInject_cv                               0.41 %          0.41 %             5
// BM_HttpTraceContextInjectEmptyContext_mean                 3.61 ns         3.61 ns            5
// BM_HttpTraceContextInjectEmptyContext_median               3.61 ns         3.61 ns            5
// BM_HttpTraceContextInjectEmptyContext_stddev              0.014 ns        0.014 ns            5
// BM_HttpTraceContextInjectEmptyContext_cv                   0.39 %          0.38 %             5
// BM_HttpTraceContextInjectWithTraceState_mean               60.0 ns         60.0 ns            5
// BM_HttpTraceContextInjectWithTraceState_median             60.0 ns         60.0 ns            5
// BM_HttpTraceContextInjectWithTraceState_stddev            0.384 ns        0.383 ns            5
// BM_HttpTraceContextInjectWithTraceState_cv                 0.64 %          0.64 %             5
// BM_HttpTraceContextExtract_mean                             156 ns          156 ns            5
// BM_HttpTraceContextExtract_median                           157 ns          157 ns            5
// BM_HttpTraceContextExtract_stddev                          2.56 ns         2.56 ns            5
// BM_HttpTraceContextExtract_cv                              1.64 %          1.64 %             5
// BM_HttpTraceContextExtractEmptyContext_mean                21.1 ns         21.1 ns            5
// BM_HttpTraceContextExtractEmptyContext_median              21.0 ns         21.0 ns            5
// BM_HttpTraceContextExtractEmptyContext_stddev             0.277 ns        0.277 ns            5
// BM_HttpTraceContextExtractEmptyContext_cv                  1.31 %          1.31 %             5
// BM_HttpTraceContextExtractWithTraceState_mean               223 ns          223 ns            5
// BM_HttpTraceContextExtractWithTraceState_median             224 ns          224 ns            5
// BM_HttpTraceContextExtractWithTraceState_stddev            4.11 ns         4.10 ns            5
// BM_HttpTraceContextExtractWithTraceState_cv                1.84 %          1.84 %             5
// BM_B3PropagatorInject_mean                                 18.9 ns         18.9 ns            5
// BM_B3PropagatorInject_median                               18.9 ns         18.9 ns            5
// BM_B3PropagatorInject_stddev                              0.336 ns        0.337 ns            5
// BM_B3PropagatorInject_cv                                   1.78 %          1.78 %             5
// BM_B3PropagatorInjectEmptyContext_mean                     3.74 ns         3.74 ns            5
// BM_B3PropagatorInjectEmptyContext_median                   3.72 ns         3.72 ns            5
// BM_B3PropagatorInjectEmptyContext_stddev                  0.052 ns        0.052 ns            5
// BM_B3PropagatorInjectEmptyContext_cv                       1.39 %          1.39 %             5
// BM_B3PropagatorExtract_mean                                 100 ns          100 ns            5
// BM_B3PropagatorExtract_median                             100.0 ns        100.0 ns            5
// BM_B3PropagatorExtract_stddev                             0.583 ns        0.583 ns            5
// BM_B3PropagatorExtract_cv                                  0.58 %          0.58 %             5
// BM_B3PropagatorExtractEmptyContext_mean                    35.2 ns         35.2 ns            5
// BM_B3PropagatorExtractEmptyContext_median                  35.2 ns         35.2 ns            5
// BM_B3PropagatorExtractEmptyContext_stddev                 0.302 ns        0.302 ns            5
// BM_B3PropagatorExtractEmptyContext_cv                      0.86 %          0.86 %             5
// BM_B3PropagatorMultiHeaderInject_mean                      29.5 ns         29.5 ns            5
// BM_B3PropagatorMultiHeaderInject_median                    29.4 ns         29.4 ns            5
// BM_B3PropagatorMultiHeaderInject_stddev                   0.173 ns        0.173 ns            5
// BM_B3PropagatorMultiHeaderInject_cv                        0.59 %          0.59 %             5
// BM_B3PropagatorMultiHeaderInjectEmptyContext_mean          3.69 ns         3.69 ns            5
// BM_B3PropagatorMultiHeaderInjectEmptyContext_median        3.70 ns         3.70 ns            5
// BM_B3PropagatorMultiHeaderInjectEmptyContext_stddev       0.018 ns        0.018 ns            5
// BM_B3PropagatorMultiHeaderInjectEmptyContext_cv            0.50 %          0.49 %             5
// BM_B3PropagatorMultiHeaderExtract_mean                     97.4 ns         97.4 ns            5
// BM_B3PropagatorMultiHeaderExtract_median                   97.1 ns         97.1 ns            5
// BM_B3PropagatorMultiHeaderExtract_stddev                  0.575 ns        0.578 ns            5
// BM_B3PropagatorMultiHeaderExtract_cv                       0.59 %          0.59 %             5
// BM_B3PropagatorMultiHeaderExtractEmptyContext_mean         38.9 ns         38.9 ns            5
// BM_B3PropagatorMultiHeaderExtractEmptyContext_median       38.9 ns         38.9 ns            5
// BM_B3PropagatorMultiHeaderExtractEmptyContext_stddev      0.318 ns        0.318 ns            5
// BM_B3PropagatorMultiHeaderExtractEmptyContext_cv           0.82 %          0.82 %             5
// BM_JaegerPropagatorInject_mean                             19.1 ns         19.1 ns            5
// BM_JaegerPropagatorInject_median                           19.2 ns         19.2 ns            5
// BM_JaegerPropagatorInject_stddev                          0.251 ns        0.251 ns            5
// BM_JaegerPropagatorInject_cv                               1.32 %          1.31 %             5
// BM_JaegerPropagatorInjectEmptyContext_mean                 3.84 ns         3.84 ns            5
// BM_JaegerPropagatorInjectEmptyContext_median               3.85 ns         3.85 ns            5
// BM_JaegerPropagatorInjectEmptyContext_stddev              0.023 ns        0.023 ns            5
// BM_JaegerPropagatorInjectEmptyContext_cv                   0.60 %          0.60 %             5
// BM_JaegerPropagatorExtract_mean                             106 ns          106 ns            5
// BM_JaegerPropagatorExtract_median                           107 ns          107 ns            5
// BM_JaegerPropagatorExtract_stddev                          1.81 ns         1.81 ns            5
// BM_JaegerPropagatorExtract_cv                              1.71 %          1.71 %             5
// BM_JaegerPropagatorExtractEmptyContext_mean                23.0 ns         23.0 ns            5
// BM_JaegerPropagatorExtractEmptyContext_median              22.8 ns         22.8 ns            5
// BM_JaegerPropagatorExtractEmptyContext_stddev             0.474 ns        0.475 ns            5
// BM_JaegerPropagatorExtractEmptyContext_cv                  2.06 %          2.06 %             5
// BM_BaggagePropagatorInject_mean                             172 ns          172 ns            5
// BM_BaggagePropagatorInject_median                           172 ns          172 ns            5
// BM_BaggagePropagatorInject_stddev                          1.57 ns         1.58 ns            5
// BM_BaggagePropagatorInject_cv                              0.91 %          0.92 %             5
// BM_BaggagePropagatorExtract_mean                            488 ns          488 ns            5
// BM_BaggagePropagatorExtract_median                          494 ns          494 ns            5
// BM_BaggagePropagatorExtract_stddev                         8.24 ns         8.25 ns            5
// BM_BaggagePropagatorExtract_cv                             1.69 %          1.69 %             5
// BM_CompositeTraceContextBaggageInject_mean                  197 ns          197 ns            5
// BM_CompositeTraceContextBaggageInject_median                198 ns          198 ns            5
// BM_CompositeTraceContextBaggageInject_stddev               3.30 ns         3.31 ns            5
// BM_CompositeTraceContextBaggageInject_cv                   1.67 %          1.68 %             5
// BM_CompositeTraceContextBaggageExtract_mean                 718 ns          718 ns            5
// BM_CompositeTraceContextBaggageExtract_median               718 ns          718 ns            5
// BM_CompositeTraceContextBaggageExtract_stddev              1.58 ns         1.59 ns            5
// BM_CompositeTraceContextBaggageExtract_cv                  0.22 %          0.22 %             5
//
// clang-format on

#include <benchmark/benchmark.h>

#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"

using namespace opentelemetry;
using namespace opentelemetry::trace::propagation;

namespace
{

class TraceContextCarrier : public context::propagation::TextMapCarrier
{
public:
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    if (key == kTraceParent)
    {
      return trace_parent_;
    }
    if (key == kTraceState)
    {
      return trace_state_;
    }
    return {};
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    if (key == kTraceParent)
    {
      trace_parent_.assign(value.empty() ? "" : value.data(), value.size());
    }
    else if (key == kTraceState)
    {
      trace_state_.assign(value.empty() ? "" : value.data(), value.size());
    }
  }

private:
  std::string trace_parent_;
  std::string trace_state_;
};

class SingleHeaderCarrier : public context::propagation::TextMapCarrier
{
public:
  explicit SingleHeaderCarrier(nostd::string_view key) : key_(key) {}

  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    return key == key_ ? nostd::string_view(value_) : nostd::string_view{};
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    if (key == key_)
    {
      value_.assign(value.empty() ? "" : value.data(), value.size());
    }
  }

private:
  nostd::string_view key_;
  std::string value_;
};

class B3MultiHeaderCarrier : public context::propagation::TextMapCarrier
{
public:
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    if (key == kB3TraceIdHeader)
    {
      return trace_id_;
    }
    if (key == kB3SpanIdHeader)
    {
      return span_id_;
    }
    if (key == kB3SampledHeader)
    {
      return sampled_;
    }
    return {};
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    if (key == kB3TraceIdHeader)
    {
      trace_id_.assign(value.empty() ? "" : value.data(), value.size());
    }
    else if (key == kB3SpanIdHeader)
    {
      span_id_.assign(value.empty() ? "" : value.data(), value.size());
    }
    else if (key == kB3SampledHeader)
    {
      sampled_.assign(value.empty() ? "" : value.data(), value.size());
    }
  }

private:
  std::string trace_id_;
  std::string span_id_;
  std::string sampled_;
};

class TraceContextBaggageCarrier : public TraceContextCarrier
{
public:
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    return key == baggage::kBaggageHeader ? nostd::string_view(baggage_)
                                          : TraceContextCarrier::Get(key);
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    if (key == baggage::kBaggageHeader)
    {
      baggage_.assign(value.empty() ? "" : value.data(), value.size());
    }
    else
    {
      TraceContextCarrier::Set(key, value);
    }
  }

private:
  std::string baggage_;
};

context::Context TraceContext(const std::string &trace_state = "")
{
  trace::SpanContext span_context(
      HttpTraceContext::TraceIdFromHex("4bf92f3577b34da6a3ce929d0e0e4736"),
      HttpTraceContext::SpanIdFromHex("0102030405060708"), trace::TraceFlags(1), false,
      trace::TraceState::FromHeader(trace_state));
  context::Context base;
  return trace::SetSpan(base, nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(span_context)));
}

void BM_HttpTraceContextInject(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  auto input = TraceContext();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextInject);

void BM_HttpTraceContextInjectEmptyContext(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  context::Context input;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    benchmark::DoNotOptimize(carrier);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextInjectEmptyContext);

void BM_HttpTraceContextInjectWithTraceState(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  auto input = TraceContext(
      "congo=t61rcWkgMzE0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextInjectWithTraceState);

void BM_HttpTraceContextExtract(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  carrier.Set(kTraceParent, "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextExtract);

void BM_HttpTraceContextExtractEmptyContext(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextExtractEmptyContext);

void BM_HttpTraceContextExtractWithTraceState(benchmark::State &state)
{
  HttpTraceContext propagator;
  TraceContextCarrier carrier;
  carrier.Set(kTraceParent, "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01");
  carrier.Set(kTraceState,
              "congo=t61rcWkgMzE0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_HttpTraceContextExtractWithTraceState);

void BM_B3PropagatorInject(benchmark::State &state)
{
  B3Propagator propagator;
  SingleHeaderCarrier carrier(kB3CombinedHeader);
  auto input = TraceContext();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorInject);

void BM_B3PropagatorInjectEmptyContext(benchmark::State &state)
{
  B3Propagator propagator;
  SingleHeaderCarrier carrier(kB3CombinedHeader);
  context::Context input;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    benchmark::DoNotOptimize(carrier);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorInjectEmptyContext);

void BM_B3PropagatorExtract(benchmark::State &state)
{
  B3Propagator propagator;
  SingleHeaderCarrier carrier(kB3CombinedHeader);
  carrier.Set(kB3CombinedHeader, "4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-1");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorExtract);

void BM_B3PropagatorExtractEmptyContext(benchmark::State &state)
{
  B3Propagator propagator;
  SingleHeaderCarrier carrier(kB3CombinedHeader);
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorExtractEmptyContext);

void BM_B3PropagatorMultiHeaderInject(benchmark::State &state)
{
  B3PropagatorMultiHeader propagator;
  B3MultiHeaderCarrier carrier;
  auto input = TraceContext();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorMultiHeaderInject);

void BM_B3PropagatorMultiHeaderInjectEmptyContext(benchmark::State &state)
{
  B3PropagatorMultiHeader propagator;
  B3MultiHeaderCarrier carrier;
  context::Context input;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    benchmark::DoNotOptimize(carrier);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorMultiHeaderInjectEmptyContext);

void BM_B3PropagatorMultiHeaderExtract(benchmark::State &state)
{
  B3PropagatorMultiHeader propagator;
  B3MultiHeaderCarrier carrier;
  carrier.Set(kB3TraceIdHeader, "4bf92f3577b34da6a3ce929d0e0e4736");
  carrier.Set(kB3SpanIdHeader, "0102030405060708");
  carrier.Set(kB3SampledHeader, "1");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorMultiHeaderExtract);

void BM_B3PropagatorMultiHeaderExtractEmptyContext(benchmark::State &state)
{
  B3PropagatorMultiHeader propagator;
  B3MultiHeaderCarrier carrier;
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_B3PropagatorMultiHeaderExtractEmptyContext);

void BM_JaegerPropagatorInject(benchmark::State &state)
{
  JaegerPropagator propagator;
  SingleHeaderCarrier carrier(kJaegerTraceHeader);
  auto input = TraceContext();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JaegerPropagatorInject);

void BM_JaegerPropagatorInjectEmptyContext(benchmark::State &state)
{
  JaegerPropagator propagator;
  SingleHeaderCarrier carrier(kJaegerTraceHeader);
  context::Context input;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    benchmark::DoNotOptimize(carrier);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JaegerPropagatorInjectEmptyContext);

void BM_JaegerPropagatorExtract(benchmark::State &state)
{
  JaegerPropagator propagator;
  SingleHeaderCarrier carrier(kJaegerTraceHeader);
  carrier.Set(kJaegerTraceHeader, "4bf92f3577b34da6a3ce929d0e0e4736:0102030405060708:0:01");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JaegerPropagatorExtract);

void BM_JaegerPropagatorExtractEmptyContext(benchmark::State &state)
{
  JaegerPropagator propagator;
  SingleHeaderCarrier carrier(kJaegerTraceHeader);
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_JaegerPropagatorExtractEmptyContext);

void BM_BaggagePropagatorInject(benchmark::State &state)
{
  baggage::propagation::BaggagePropagator propagator;
  SingleHeaderCarrier carrier(baggage::kBaggageHeader);
  context::Context base;
  auto input = baggage::SetBaggage(
      base, baggage::Baggage::FromHeader(
                "tenant=0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_BaggagePropagatorInject);

void BM_BaggagePropagatorExtract(benchmark::State &state)
{
  baggage::propagation::BaggagePropagator propagator;
  SingleHeaderCarrier carrier(baggage::kBaggageHeader);
  carrier.Set(baggage::kBaggageHeader,
              "tenant=0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_BaggagePropagatorExtract);

void BM_CompositeTraceContextBaggageInject(benchmark::State &state)
{
  std::vector<std::unique_ptr<context::propagation::TextMapPropagator>> propagators;
  propagators.emplace_back(new HttpTraceContext);
  propagators.emplace_back(new baggage::propagation::BaggagePropagator);
  context::propagation::CompositePropagator propagator(std::move(propagators));
  TraceContextBaggageCarrier carrier;
  auto base  = TraceContext();
  auto input = baggage::SetBaggage(
      base, baggage::Baggage::FromHeader(
                "tenant=0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(input);
    propagator.Inject(carrier, input);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_CompositeTraceContextBaggageInject);

void BM_CompositeTraceContextBaggageExtract(benchmark::State &state)
{
  std::vector<std::unique_ptr<context::propagation::TextMapPropagator>> propagators;
  propagators.emplace_back(new HttpTraceContext);
  propagators.emplace_back(new baggage::propagation::BaggagePropagator);
  context::propagation::CompositePropagator propagator(std::move(propagators));
  TraceContextBaggageCarrier carrier;
  carrier.Set(kTraceParent, "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01");
  carrier.Set(baggage::kBaggageHeader,
              "tenant=0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  context::Context base;
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(carrier);
    benchmark::DoNotOptimize(base);
    auto extracted = propagator.Extract(carrier, base);
    benchmark::DoNotOptimize(extracted);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_CompositeTraceContextBaggageExtract);
}  // namespace

BENCHMARK_MAIN();
