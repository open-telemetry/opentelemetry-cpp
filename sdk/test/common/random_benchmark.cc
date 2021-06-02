// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "src/common/random.h"

#include <cstdint>
#include <random>

#include <benchmark/benchmark.h>

namespace
{
using opentelemetry::sdk::common::Random;

void BM_RandomIdGeneration(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(Random::GenerateRandom64());
  }
}
BENCHMARK(BM_RandomIdGeneration);

void BM_RandomIdStdGeneration(benchmark::State &state)
{
  std::mt19937_64 generator{0};
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(generator());
  }
}
BENCHMARK(BM_RandomIdStdGeneration);

}  // namespace
BENCHMARK_MAIN();
