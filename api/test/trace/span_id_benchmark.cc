// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/trace/span_id.h"

#include <benchmark/benchmark.h>
#include <cstdint>

namespace
{
using opentelemetry::trace::SpanId;
constexpr uint8_t bytes[] = {1, 2, 3, 4, 5, 6, 7, 8};

void BM_SpanIdDefaultConstructor(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(SpanId());
  }
}
BENCHMARK(BM_SpanIdDefaultConstructor);

void BM_SpanIdConstructor(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(SpanId(bytes));
  }
}
BENCHMARK(BM_SpanIdConstructor);

void BM_SpanIdToLowerBase16(benchmark::State &state)
{
  SpanId id(bytes);
  char buf[SpanId::kSize * 2];
  while (state.KeepRunning())
  {
    id.ToLowerBase16(buf);
    benchmark::DoNotOptimize(buf);
  }
}
BENCHMARK(BM_SpanIdToLowerBase16);

void BM_SpanIdIsValid(benchmark::State &state)
{
  SpanId id(bytes);
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(id.IsValid());
  }
}
BENCHMARK(BM_SpanIdIsValid);

}  // namespace
BENCHMARK_MAIN();
