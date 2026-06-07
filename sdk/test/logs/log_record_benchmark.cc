// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <stdint.h>
#include <array>
#include <cstddef>
#include <limits>
#include <string>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"

namespace logs_sdk = opentelemetry::sdk::logs;
namespace nostd    = opentelemetry::nostd;

namespace
{

constexpr const char *kAttributeKey = "benchmark.attribute";
constexpr std::size_t kSmallLimit   = 8;
constexpr std::size_t kLargeLimit   = 1024;

void ConfigureLogRecordLimits(logs_sdk::ReadWriteLogRecord &record,
                              std::size_t attribute_value_length_limit)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_value_length_limit = attribute_value_length_limit;
  record.SetLogRecordLimits(limits);
}

void BenchmarkStringAttribute(benchmark::State &state,
                              std::size_t attribute_value_length_limit,
                              nostd::string_view value)
{
  logs_sdk::ReadWriteLogRecord record;
  ConfigureLogRecordLimits(record, attribute_value_length_limit);
  record.SetAttribute(kAttributeKey, value);

  for (auto _ : state)
  {
    record.SetAttribute(kAttributeKey, value);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(value.size()));
}

static void BM_SetStringAttributeNoLimit(benchmark::State &state)
{
  const std::string value(2048, 'a');
  BenchmarkStringAttribute(state, (std::numeric_limits<std::size_t>::max)(), value);
}
BENCHMARK(BM_SetStringAttributeNoLimit);

static void BM_SetStringAttributeLimitNotHit(benchmark::State &state)
{
  const std::string value(2048, 'a');
  BenchmarkStringAttribute(state, value.size(), value);
}
BENCHMARK(BM_SetStringAttributeLimitNotHit);

static void BM_SetStringAttributeSmallLimitHit(benchmark::State &state)
{
  const std::string value(2048, 'a');
  BenchmarkStringAttribute(state, kSmallLimit, value);
}
BENCHMARK(BM_SetStringAttributeSmallLimitHit);

static void BM_SetStringAttributeLargeLimitHit(benchmark::State &state)
{
  const std::string value(2048, 'a');
  BenchmarkStringAttribute(state, kLargeLimit, value);
}
BENCHMARK(BM_SetStringAttributeLargeLimitHit);

template <std::size_t N>
void BenchmarkStringArrayAttributeLimitHit(benchmark::State &state,
                                           const std::array<nostd::string_view, N> &values)
{
  auto values_span = nostd::span<const nostd::string_view>(values.data(), values.size());

  logs_sdk::ReadWriteLogRecord record;
  ConfigureLogRecordLimits(record, kSmallLimit);
  record.SetAttribute(kAttributeKey, values_span);

  for (auto _ : state)
  {
    record.SetAttribute(kAttributeKey, values_span);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(values.size()));
}

static void BM_SetStringArrayAttributeShortArrayLimitHit(benchmark::State &state)
{
  const std::string short_value(16, 'a');
  const std::string long_value(2048, 'b');
  const std::array<nostd::string_view, 4> values = {
      nostd::string_view(short_value),
      nostd::string_view(long_value),
      nostd::string_view(short_value),
      nostd::string_view(long_value),
  };

  BenchmarkStringArrayAttributeLimitHit(state, values);
}
BENCHMARK(BM_SetStringArrayAttributeShortArrayLimitHit);

static void BM_SetStringArrayAttributeLongArrayLimitHit(benchmark::State &state)
{
  const std::string short_value(16, 'a');
  const std::string long_value(2048, 'b');
  const std::array<nostd::string_view, 64> values = {
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
      nostd::string_view(short_value), nostd::string_view(long_value),
  };

  BenchmarkStringArrayAttributeLimitHit(state, values);
}
BENCHMARK(BM_SetStringArrayAttributeLongArrayLimitHit);

}  // namespace

BENCHMARK_MAIN();
