// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/logger_provider.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/sdk/trace/simple_processor_factory.h"
#  include "opentelemetry/sdk/trace/tracer_provider.h"
#  include "opentelemetry/trace/span.h"
#  include "opentelemetry/trace/tracer.h"
#endif

#if OPENTELEMETRY_HAVE_EXCEPTIONS
namespace
{
thread_local std::size_t failure_size       = 0;
thread_local int allocations_before_failure = -1;

class FailAllocation
{
public:
  explicit FailAllocation(std::size_t size = 0, int skip = 0)
      : previous_size_(std::exchange(failure_size, size)),
        previous_count_(std::exchange(allocations_before_failure, skip))
  {}
  ~FailAllocation()
  {
    failure_size               = previous_size_;
    allocations_before_failure = previous_count_;
  }
  FailAllocation(const FailAllocation &)            = delete;
  FailAllocation &operator=(const FailAllocation &) = delete;
  FailAllocation(FailAllocation &&)                 = delete;
  FailAllocation &operator=(FailAllocation &&)      = delete;

private:
  std::size_t previous_size_;
  int previous_count_;
};

constexpr std::size_t kLongValueSize = 4096;
}  // namespace

// Keep allocation injection in this dedicated test executable.
void *operator new(std::size_t size)
{
  // A threshold also covers standard libraries that round string capacities up.
  if (allocations_before_failure >= 0 && (failure_size == 0 || size >= failure_size) &&
      allocations_before_failure-- == 0)
  {
    allocations_before_failure = -1;
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

namespace
{
namespace common = opentelemetry::common;
namespace nostd  = opentelemetry::nostd;
using opentelemetry::sdk::common::AttributeMap;
using opentelemetry::sdk::common::OrderedAttributeMap;
using opentelemetry::sdk::instrumentationscope::InstrumentationScope;

// Fail inside the noexcept iterable callback, after one attribute was copied.
class FailingAttributes : public common::KeyValueIterable
{
public:
  bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
                           callback) const noexcept override
  {
    if (!callback("first", true))
    {
      return false;
    }
    FailAllocation failure(kLongValueSize + 1);
    return callback("second", nostd::string_view(value_));
  }
  std::size_t size() const noexcept override { return 2; }

private:
  std::string value_ = std::string(kLongValueSize, 'x');
};
}  // namespace

TEST(AttributeAllocation, KeyAllocationReturnsFalse)
{
  AttributeMap attributes;
  std::string key(kLongValueSize, 'k');
  bool stored = false;
  {
    FailAllocation failure(kLongValueSize + 1);
    stored = attributes.SetAttribute(key, true);
  }
  EXPECT_FALSE(stored);
  EXPECT_TRUE(attributes.empty());
}

TEST(AttributeAllocation, NodeAllocationReturnsFalse)
{
  AttributeMap attributes;
  attributes.reserve(1);
  bool stored = false;
  {
    FailAllocation failure;
    stored = attributes.SetAttribute("key", true);
  }
  EXPECT_FALSE(stored);
  EXPECT_TRUE(attributes.empty());
}

TEST(AttributeAllocation, BucketAllocationReturnsFalse)
{
  AttributeMap attributes;
  bool stored = false;
  {
    FailAllocation failure(0, 1);  // The node succeeds; allocating buckets fails.
    stored = attributes.SetAttribute("key", true);
  }
  EXPECT_FALSE(stored);
  EXPECT_TRUE(attributes.empty());
}

TEST(AttributeAllocation, ValueAllocationReturnsFalse)
{
  AttributeMap attributes;
  std::string value(kLongValueSize, 'x');
  bool stored = false;
  {
    FailAllocation failure(kLongValueSize + 1);
    stored = attributes.SetAttribute("key", nostd::string_view(value));
  }
  EXPECT_FALSE(stored);
  EXPECT_TRUE(attributes.empty());
}

TEST(AttributeAllocation, OrderedMapInsertionDoesNotTerminate)
{
  OrderedAttributeMap attributes;
  std::string key(kLongValueSize, 'k');
  {
    FailAllocation failure(kLongValueSize + 1);
    attributes.SetAttribute(key, true);
  }
  EXPECT_TRUE(attributes.empty());
}

TEST(AttributeAllocation, ScopeSetAttributeDoesNotTerminate)
{
  auto scope = InstrumentationScope::Create("test");
  std::string key(kLongValueSize, 'k');
  {
    FailAllocation failure(kLongValueSize + 1);
    scope->SetAttribute(key, true);
  }
  EXPECT_TRUE(scope->GetAttributes().empty());
}

TEST(AttributeAllocation, IterableConstructionRejectsPartialAttributes)
{
  FailingAttributes attributes;
  EXPECT_THROW((void)AttributeMap(attributes), std::bad_alloc);
}

TEST(AttributeAllocation, InitializerListConstructionRejectsPartialAttributes)
{
  std::string value(kLongValueSize, 'x');
  FailAllocation failure(kLongValueSize + 1);
  EXPECT_THROW((void)AttributeMap({{"first", true}, {"second", nostd::string_view(value)}}),
               std::bad_alloc);
}

TEST(AttributeAllocation, ScopeConstructionRejectsPartialAttributes)
{
  std::string value(kLongValueSize, 'x');
  std::vector<std::pair<nostd::string_view, common::AttributeValue>> attributes{
      {"first", true}, {"second", nostd::string_view(value)}};
  FailAllocation failure(kLongValueSize + 1);
  EXPECT_THROW(InstrumentationScope::Create("test", "", "", attributes), std::bad_alloc);
}

TEST(AttributeAllocation, LoggerProviderReturnsNoop)
{
  opentelemetry::sdk::logs::LoggerProvider provider;
  FailingAttributes attributes;
  auto cached = provider.GetLogger("cached", "cached");
  auto failed = provider.GetLogger("failed", "failed", "", "", attributes);
  ASSERT_NE(failed, nullptr);
  EXPECT_EQ(failed->GetName(), "noop logger");
  EXPECT_EQ(provider.GetLogger("cached", "cached"), cached);
  EXPECT_EQ(provider.GetLogger("other", "other"), failed);
}

#  if OPENTELEMETRY_ABI_VERSION_NO >= 2
TEST(AttributeAllocation, TracerProviderReturnsNoop)
{
  opentelemetry::sdk::trace::TracerProvider provider(
      opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(nullptr));
  FailingAttributes attributes;
  auto cached = provider.GetTracer("cached");
  auto failed = provider.GetTracer("failed", "", "", &attributes);
  ASSERT_NE(failed, nullptr);
  EXPECT_FALSE(failed->StartSpan("noop")->IsRecording());
  EXPECT_EQ(provider.GetTracer("cached"), cached);
  EXPECT_EQ(provider.GetTracer("other"), failed);
}

TEST(AttributeAllocation, MeterProviderReturnsNoop)
{
  opentelemetry::sdk::metrics::MeterProvider provider;
  FailingAttributes attributes;
  auto cached = provider.GetMeter("cached");
  auto failed = provider.GetMeter("failed", "", "", &attributes);
  ASSERT_NE(failed, nullptr);
  EXPECT_NE(failed, cached);
  EXPECT_EQ(provider.GetMeter("cached"), cached);
  EXPECT_EQ(provider.GetMeter("other"), failed);
}
#  endif
#endif  // OPENTELEMETRY_HAVE_EXCEPTIONS
