// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off

// ~/build/api/test/trace/propagation/propagator_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-10-12T16:13:28+00:00
// Running /home/devuser/build/api/test/trace/propagation/propagator_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.02, 0.76, 0.61
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// ------------------------------------------------------------------------------------------
// Benchmark                                                Time             CPU   Iterations
// ------------------------------------------------------------------------------------------
// BM_HttpTraceContextInject_mean                        19.5 ns         19.5 ns            5
// BM_HttpTraceContextInject_median                      19.4 ns         19.4 ns            5
// BM_HttpTraceContextInject_stddev                     0.312 ns        0.312 ns            5
// BM_HttpTraceContextInject_cv                          1.60 %          1.60 %             5
// BM_HttpTraceContextInjectWithTraceState_mean          55.0 ns         54.9 ns            5
// BM_HttpTraceContextInjectWithTraceState_median        54.8 ns         54.8 ns            5
// BM_HttpTraceContextInjectWithTraceState_stddev       0.312 ns        0.312 ns            5
// BM_HttpTraceContextInjectWithTraceState_cv            0.57 %          0.57 %             5
// BM_HttpTraceContextExtract_mean                        158 ns          158 ns            5
// BM_HttpTraceContextExtract_median                      157 ns          157 ns            5
// BM_HttpTraceContextExtract_stddev                     1.83 ns         1.83 ns            5
// BM_HttpTraceContextExtract_cv                         1.16 %          1.16 %             5
// BM_HttpTraceContextExtractWithTraceState_mean          222 ns          222 ns            5
// BM_HttpTraceContextExtractWithTraceState_median        222 ns          222 ns            5
// BM_HttpTraceContextExtractWithTraceState_stddev       1.97 ns         1.95 ns            5
// BM_HttpTraceContextExtractWithTraceState_cv           0.89 %          0.88 %             5
// BM_B3PropagatorInject_mean                            18.6 ns         18.6 ns            5
// BM_B3PropagatorInject_median                          18.6 ns         18.6 ns            5
// BM_B3PropagatorInject_stddev                         0.083 ns        0.084 ns            5
// BM_B3PropagatorInject_cv                              0.45 %          0.45 %             5
// BM_B3PropagatorExtract_mean                            103 ns          103 ns            5
// BM_B3PropagatorExtract_median                          103 ns          103 ns            5
// BM_B3PropagatorExtract_stddev                        0.433 ns        0.439 ns            5
// BM_B3PropagatorExtract_cv                             0.42 %          0.43 %             5
// BM_B3PropagatorMultiHeaderInject_mean                 29.1 ns         29.1 ns            5
// BM_B3PropagatorMultiHeaderInject_median               29.1 ns         29.1 ns            5
// BM_B3PropagatorMultiHeaderInject_stddev              0.210 ns        0.211 ns            5
// BM_B3PropagatorMultiHeaderInject_cv                   0.72 %          0.72 %             5
// BM_B3PropagatorMultiHeaderExtract_mean                89.3 ns         89.2 ns            5
// BM_B3PropagatorMultiHeaderExtract_median              89.1 ns         89.1 ns            5
// BM_B3PropagatorMultiHeaderExtract_stddev             0.524 ns        0.527 ns            5
// BM_B3PropagatorMultiHeaderExtract_cv                  0.59 %          0.59 %             5
// BM_JaegerPropagatorInject_mean                        18.6 ns         18.6 ns            5
// BM_JaegerPropagatorInject_median                      18.5 ns         18.5 ns            5
// BM_JaegerPropagatorInject_stddev                     0.242 ns        0.242 ns            5
// BM_JaegerPropagatorInject_cv                          1.30 %          1.30 %             5
// BM_JaegerPropagatorExtract_mean                       98.2 ns         98.2 ns            5
// BM_JaegerPropagatorExtract_median                     97.7 ns         97.7 ns            5
// BM_JaegerPropagatorExtract_stddev                     2.16 ns         2.17 ns            5
// BM_JaegerPropagatorExtract_cv                         2.20 %          2.21 %             5
// BM_BaggagePropagatorInject_mean                        169 ns          169 ns            5
// BM_BaggagePropagatorInject_median                      169 ns          169 ns            5
// BM_BaggagePropagatorInject_stddev                    0.384 ns        0.396 ns            5
// BM_BaggagePropagatorInject_cv                         0.23 %          0.24 %             5
// BM_BaggagePropagatorExtract_mean                       425 ns          425 ns            5
// BM_BaggagePropagatorExtract_median                     422 ns          422 ns            5
// BM_BaggagePropagatorExtract_stddev                    7.84 ns         7.85 ns            5
// BM_BaggagePropagatorExtract_cv                        1.85 %          1.85 %             5
// BM_CompositeTraceContextBaggageInject_mean             189 ns          189 ns            5
// BM_CompositeTraceContextBaggageInject_median           189 ns          189 ns            5
// BM_CompositeTraceContextBaggageInject_stddev         0.830 ns        0.826 ns            5
// BM_CompositeTraceContextBaggageInject_cv              0.44 %          0.44 %             5
// BM_CompositeTraceContextBaggageExtract_mean            619 ns          619 ns            5
// BM_CompositeTraceContextBaggageExtract_median          619 ns          619 ns            5
// BM_CompositeTraceContextBaggageExtract_stddev         4.59 ns         4.62 ns            5
// BM_CompositeTraceContextBaggageExtract_cv             0.74 %          0.75 %             5
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
