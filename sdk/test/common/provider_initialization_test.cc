// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstddef>  // IWYU pragma: keep
#include <cstdlib>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/macros.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/multi_log_record_processor.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/id_generator.h"
#include "opentelemetry/sdk/trace/multi_span_processor.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/random_id_generator.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

#if OPENTELEMETRY_HAVE_EXCEPTIONS
namespace
{
thread_local std::size_t failure_size = 0;

class FailAllocation
{
public:
  explicit FailAllocation(std::size_t size) : previous_(std::exchange(failure_size, size)) {}
  ~FailAllocation() { failure_size = previous_; }
  FailAllocation(const FailAllocation &)            = delete;
  FailAllocation &operator=(const FailAllocation &) = delete;
  FailAllocation(FailAllocation &&)                 = delete;
  FailAllocation &operator=(FailAllocation &&)      = delete;

private:
  std::size_t previous_;
};

// Target the resource's schema string, not variant-copy cleanup or constructor arguments.
constexpr std::size_t kResourceValueSize = 4096;
}  // namespace

// Keep allocation injection in this dedicated test executable.
// Keep the malloc/free implementations out of line to avoid GCC mismatch warnings.
#  if defined(__GNUC__) && !defined(__clang__)
[[gnu::noinline]]
#  endif
void *operator new(std::size_t size)
{
  // A threshold also covers standard libraries that round string capacities up.
  if (failure_size != 0 && size >= failure_size)
  {
    failure_size = 0;
    throw std::bad_alloc();
  }
  if (void *result = std::malloc(size == 0 ? 1 : size))  // NOLINT(cppcoreguidelines-no-malloc)
  {
    return result;
  }
  throw std::bad_alloc();
}

void *operator new[](std::size_t size)
{
  return ::operator new(size);
}
#  if defined(__GNUC__) && !defined(__clang__)
[[gnu::noinline]]
#  endif
void operator delete(void *ptr) noexcept
{
  std::free(ptr);  // NOLINT(cppcoreguidelines-no-malloc)
}
void operator delete[](void *ptr) noexcept
{
  ::operator delete(ptr);
}
void operator delete(void *ptr, std::size_t) noexcept
{
  ::operator delete(ptr);
}
void operator delete[](void *ptr, std::size_t) noexcept
{
  ::operator delete(ptr);
}
void *operator new(std::size_t size, const std::nothrow_t &) noexcept
{
  try
  {
    return ::operator new(size);
  }
  catch (const std::bad_alloc &)
  {
    return nullptr;
  }
}
void *operator new[](std::size_t size, const std::nothrow_t &tag) noexcept
{
  return ::operator new(size, tag);
}
void operator delete(void *ptr, const std::nothrow_t &) noexcept
{
  ::operator delete(ptr);
}
void operator delete[](void *ptr, const std::nothrow_t &) noexcept
{
  ::operator delete(ptr);
}

namespace trace_sdk   = opentelemetry::sdk::trace;
namespace logs_sdk    = opentelemetry::sdk::logs;
namespace metrics_sdk = opentelemetry::sdk::metrics;
using opentelemetry::sdk::resource::Resource;

TEST(ProviderInitialization, TracerContextResourceCopyThrows)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  auto sampler   = std::make_unique<trace_sdk::AlwaysOnSampler>();
  auto generator = std::make_unique<trace_sdk::RandomIdGenerator>();
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(trace_sdk::TracerContext({}, resource, std::move(sampler), std::move(generator)),
               std::bad_alloc);
}

TEST(ProviderInitialization, LoggerContextResourceCopyThrows)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(logs_sdk::LoggerContext({}, resource), std::bad_alloc);
}

TEST(ProviderInitialization, MeterContextResourceCopyThrows)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  auto views = std::make_unique<metrics_sdk::ViewRegistry>();
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(metrics_sdk::MeterContext(std::move(views), resource), std::bad_alloc);
}

TEST(ProviderInitialization, TracerProviderResourceCopyReachesCaller)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  auto sampler   = std::make_unique<trace_sdk::AlwaysOnSampler>();
  auto generator = std::make_unique<trace_sdk::RandomIdGenerator>();
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(trace_sdk::TracerProvider(std::vector<std::unique_ptr<trace_sdk::SpanProcessor>>{},
                                         resource, std::move(sampler), std::move(generator)),
               std::bad_alloc);
}

TEST(ProviderInitialization, LoggerProviderResourceCopyReachesCaller)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(logs_sdk::LoggerProvider(
                   std::vector<std::unique_ptr<logs_sdk::LogRecordProcessor>>{}, resource),
               std::bad_alloc);
}

TEST(ProviderInitialization, MeterProviderResourceCopyReachesCaller)
{
  Resource resource({{"key", true}}, std::string(kResourceValueSize, 'x'));
  auto views = std::make_unique<metrics_sdk::ViewRegistry>();
  FailAllocation failure(kResourceValueSize + 1);
  EXPECT_THROW(metrics_sdk::MeterProvider(std::move(views), resource), std::bad_alloc);
}

TEST(ProviderInitialization, TracerContextProcessorAllocationThrows)
{
  Resource resource;
  auto sampler   = std::make_unique<trace_sdk::AlwaysOnSampler>();
  auto generator = std::make_unique<trace_sdk::RandomIdGenerator>();
  using Configurator =
      opentelemetry::sdk::instrumentationscope::ScopeConfigurator<trace_sdk::TracerConfig>;
  auto configurator = std::make_unique<Configurator>(
      Configurator::Builder(trace_sdk::TracerConfig::Default()).Build());
  FailAllocation failure(sizeof(trace_sdk::MultiSpanProcessor));
  EXPECT_THROW(trace_sdk::TracerContext({}, resource, std::move(sampler), std::move(generator),
                                        std::move(configurator)),
               std::bad_alloc);
}

TEST(ProviderInitialization, LoggerContextProcessorAllocationThrows)
{
  Resource resource;
  using Configurator =
      opentelemetry::sdk::instrumentationscope::ScopeConfigurator<logs_sdk::LoggerConfig>;
  auto configurator = std::make_unique<Configurator>(
      Configurator::Builder(logs_sdk::LoggerConfig::Default()).Build());
  FailAllocation failure(sizeof(logs_sdk::MultiLogRecordProcessor));
  EXPECT_THROW(logs_sdk::LoggerContext({}, resource, std::move(configurator)), std::bad_alloc);
}
#endif  // OPENTELEMETRY_HAVE_EXCEPTIONS
