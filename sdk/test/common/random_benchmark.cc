#include "src/common/random.h"

#include <benchmark/benchmark.h>
#include <cstdint>

namespace
{
using opentelemetry::sdk::common::GetRandomNumberGenerator;

void BM_RandomIdGeneration(benchmark::State &state)
{
  auto &generator = GetRandomNumberGenerator();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(generator());
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
