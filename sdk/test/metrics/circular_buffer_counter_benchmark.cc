// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <stddef.h>
#include <stdint.h>

#include "opentelemetry/sdk/metrics/data/circular_buffer.h"

using opentelemetry::sdk::metrics::AdaptingCircularBufferCounter;

namespace
{

// Baseline for the bucket container behind the base2 exponential histogram.
// The aggregation benchmarks in histogram_aggregation_benchmark.cc exercise
// this class together with the indexer, the spin lock and the sum/min/max
// bookkeeping, which hides changes of a few percent in the container itself.
//
// See https://github.com/open-telemetry/opentelemetry-cpp/issues/3366.
//
// Every recorded count stays at one, so the backing array keeps its one-byte
// cells and no result depends on how often integer width promotion happens.
//
// The fill benchmarks reset the buffer inside the timed region because an
// iteration has to start from an empty buffer. Clear() zeroes the backing array
// without allocating and its cost does not depend on the code paths these
// benchmarks are meant to track, so it shifts every measurement by the same
// constant.

// Window sizes: the smallest configurable one, the default max_size_ of
// Base2ExponentialHistogramAggregationConfig, and a large one.
constexpr int64_t kSmallWindow   = 20;
constexpr int64_t kDefaultWindow = 160;
constexpr int64_t kLargeWindow   = 640;

// Fills the whole window from the lowest index upwards, so every index lands
// after the base index and ToBufferIndex() takes its non-wrapping branch.
void BM_AdaptingCircularBufferCounterFillAscending(benchmark::State &state)
{
  const int32_t window = static_cast<int32_t>(state.range(0));
  AdaptingCircularBufferCounter counter(static_cast<size_t>(window));

  for (auto _ : state)
  {
    counter.Clear();
    for (int32_t index = 0; index < window; ++index)
    {
      counter.Increment(index, 1);
    }
  }
  state.SetItemsProcessed(state.iterations() * window);
}
BENCHMARK(BM_AdaptingCircularBufferCounterFillAscending)
    ->Arg(kSmallWindow)
    ->Arg(kDefaultWindow)
    ->Arg(kLargeWindow);

// Fills the same window downwards from the base index, which leaves the buffer
// wrapped: everything but the first recording sits at the physical end of the
// backing array. Downscaling has to handle that layout, so it is measured
// separately from the ascending case.
void BM_AdaptingCircularBufferCounterFillWrapped(benchmark::State &state)
{
  const int32_t window = static_cast<int32_t>(state.range(0));
  AdaptingCircularBufferCounter counter(static_cast<size_t>(window));

  for (auto _ : state)
  {
    counter.Clear();
    counter.Increment(0, 1);
    for (int32_t index = -1; index > -window; --index)
    {
      counter.Increment(index, 1);
    }
  }
  state.SetItemsProcessed(state.iterations() * window);
}
BENCHMARK(BM_AdaptingCircularBufferCounterFillWrapped)
    ->Arg(kSmallWindow)
    ->Arg(kDefaultWindow)
    ->Arg(kLargeWindow);

// Reads every bucket of a wrapped buffer, which is what downscaling and the
// merge/diff paths do before writing the folded counts.
void BM_AdaptingCircularBufferCounterGetWrapped(benchmark::State &state)
{
  const int32_t window = static_cast<int32_t>(state.range(0));
  AdaptingCircularBufferCounter counter(static_cast<size_t>(window));
  counter.Increment(0, 1);
  for (int32_t index = -1; index > -window; --index)
  {
    counter.Increment(index, 1);
  }

  for (auto _ : state)
  {
    uint64_t total = 0;
    for (int32_t index = counter.StartIndex(); index <= counter.EndIndex(); ++index)
    {
      total += counter.Get(index);
    }
    benchmark::DoNotOptimize(total);
  }
  state.SetItemsProcessed(state.iterations() * window);
}
BENCHMARK(BM_AdaptingCircularBufferCounterGetWrapped)
    ->Arg(kSmallWindow)
    ->Arg(kDefaultWindow)
    ->Arg(kLargeWindow);

// Folds a full, wrapped buffer in half. This is the operation the base2
// exponential histogram runs whenever a recording falls outside the current
// window, and it is the one that used to allocate a replacement buffer per
// call.
//
// Downscaling is destructive, so an iteration cannot start from the previous
// result. The buffer is restored by copy-assigning a pristine one, which for
// equally sized buffers of the same cell width is a copy of the counts with no
// allocation, rather than by replaying the recordings.
void BM_AdaptingCircularBufferCounterDownscale(benchmark::State &state)
{
  const int32_t window = static_cast<int32_t>(state.range(0));
  AdaptingCircularBufferCounter filled(static_cast<size_t>(window));
  filled.Increment(0, 1);
  for (int32_t index = -1; index > -window; --index)
  {
    filled.Increment(index, 1);
  }

  AdaptingCircularBufferCounter counter(static_cast<size_t>(window));
  for (auto _ : state)
  {
    counter = filled;
    counter.Downscale(1);
    benchmark::DoNotOptimize(counter);
  }
  state.SetItemsProcessed(state.iterations() * window);
}
BENCHMARK(BM_AdaptingCircularBufferCounterDownscale)
    ->Arg(kSmallWindow)
    ->Arg(kDefaultWindow)
    ->Arg(kLargeWindow);

}  // namespace
BENCHMARK_MAIN();
