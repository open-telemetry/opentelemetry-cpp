// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"

using opentelemetry::context::propagation::GlobalTextMapPropagator;
using opentelemetry::context::propagation::TextMapCarrier;
using opentelemetry::context::propagation::TextMapPropagator;

namespace context = opentelemetry::context;
namespace nostd   = opentelemetry::nostd;

namespace
{

class TextMapCarrierTest : public TextMapCarrier
{
public:
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = headers_.find(std::string(key));
    if (it != headers_.end())
    {
      return nostd::string_view(it->second);
    }
    return "";
  }

  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    headers_[std::string(key)] = std::string(value);
  }

  std::map<std::string, std::string> headers_;
};

class TestPropagator : public TextMapPropagator
{
public:
  context::Context Extract(const TextMapCarrier & /* carrier */,
                           context::Context &context) noexcept override
  {
    return context;
  }

  void Inject(TextMapCarrier &carrier, const context::Context & /* context */) noexcept override
  {
    carrier.Set("test", "value");
  }

  bool Fields(nostd::function_ref<bool(nostd::string_view)> /* callback */) const noexcept override
  {
    return true;
  }
};

TEST(GlobalPropagator, GetGlobalPropagatorDefault)
{
  auto propagator = GlobalTextMapPropagator::GetGlobalPropagator();
  EXPECT_NE(nullptr, propagator);
}

TEST(GlobalPropagator, SetGlobalPropagator)
{
  auto propagator = nostd::shared_ptr<TextMapPropagator>(new TestPropagator());
  GlobalTextMapPropagator::SetGlobalPropagator(propagator);
  ASSERT_EQ(propagator, GlobalTextMapPropagator::GetGlobalPropagator());
}

TEST(GlobalPropagator, SetNullGlobalPropagator)
{
  auto propagator = nostd::shared_ptr<TextMapPropagator>(new TestPropagator());
  GlobalTextMapPropagator::SetGlobalPropagator(propagator);
  ASSERT_EQ(propagator, GlobalTextMapPropagator::GetGlobalPropagator());

  // Setting a null TextMapPropagator installs a no-op TextMapPropagator.
  GlobalTextMapPropagator::SetGlobalPropagator(nostd::shared_ptr<TextMapPropagator>());

  auto noop = GlobalTextMapPropagator::GetGlobalPropagator();
  ASSERT_NE(nullptr, noop);
  ASSERT_NE(propagator, noop);

  // The no-op TextMapPropagator is usable, it does not crash on use.
  TextMapCarrierTest carrier;
  context::Context context = context::Context{"key", static_cast<int64_t>(42)};

  // The no-op inject writes nothing to the carrier.
  noop->Inject(carrier, context);
  EXPECT_TRUE(carrier.headers_.empty());

  // The no-op extract returns the input context.
  context::Context extracted = noop->Extract(carrier, context);
  EXPECT_EQ(42, nostd::get<int64_t>(extracted.GetValue("key")));
}

}  // namespace
