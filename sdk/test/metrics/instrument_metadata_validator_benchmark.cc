// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <string>

#include "opentelemetry/sdk/metrics/instrument_metadata_validator.h"

using opentelemetry::sdk::metrics::InstrumentMetaDataValidator;

namespace
{

void BM_ConstructValidator(benchmark::State &state)
{
  for (auto _ : state)
  {
    InstrumentMetaDataValidator validator;
    benchmark::DoNotOptimize(validator);
  }
}
BENCHMARK(BM_ConstructValidator);

void BM_ValidateName(benchmark::State &state, const std::string &name)
{
  InstrumentMetaDataValidator validator;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(validator.ValidateName(name));
  }
}
BENCHMARK_CAPTURE(BM_ValidateName, Typical, std::string("http.server.request.duration"));
BENCHMARK_CAPTURE(BM_ValidateName, MaxLength, std::string(255, 'a'));

void BM_ValidateUnit(benchmark::State &state, const std::string &unit)
{
  InstrumentMetaDataValidator validator;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(validator.ValidateUnit(unit));
  }
}
BENCHMARK_CAPTURE(BM_ValidateUnit, Typical, std::string("ms"));
BENCHMARK_CAPTURE(BM_ValidateUnit, MaxLength, std::string(63, 'u'));

}  // namespace

BENCHMARK_MAIN();
