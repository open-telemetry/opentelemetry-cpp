// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"

using namespace OPENTELEMETRY_NAMESPACE;
using opentelemetry::exporter::etw::Properties;
using opentelemetry::exporter::etw::PropertyValue;

// PropertyValue / Properties are header-only converters. These tests do not
// require Windows ETW APIs or an active provider.

TEST(ETWProperties, SpanStringAttributeKeepsExactCountAndContent)
{
  const nostd::string_view views[] = {"alpha", "beta", "gamma"};
  common::AttributeValue attr{nostd::span<const nostd::string_view>{views}};

  PropertyValue value;
  value.FromAttributeValue(attr);

  ASSERT_EQ(value.index(), static_cast<size_t>(exporter::etw::kTypeSpanString));
  const auto &owned = nostd::get<std::vector<std::string>>(value);
  ASSERT_EQ(owned.size(), 3u);
  EXPECT_EQ(owned[0], "alpha");
  EXPECT_EQ(owned[1], "beta");
  EXPECT_EQ(owned[2], "gamma");
}

TEST(ETWProperties, SpanStringAttributeCopiesByViewLengthNotNul)
{
  // Buffer continues past each logical view. A NUL-scanning copy would overrun
  // or include trailing characters; a length-aware copy must not.
  const char buffer[]              = {'a', 'b', 'c', 'X', 'Y', 'Z', '1', '2', '3'};
  const nostd::string_view views[] = {
      nostd::string_view(buffer, 3),      // "abc"
      nostd::string_view(buffer + 3, 3),  // "XYZ"
      nostd::string_view(buffer + 6, 3),  // "123"
  };
  common::AttributeValue attr{nostd::span<const nostd::string_view>{views}};

  PropertyValue value;
  value.FromAttributeValue(attr);

  const auto &owned = nostd::get<std::vector<std::string>>(value);
  ASSERT_EQ(owned.size(), 3u);
  EXPECT_EQ(owned[0], "abc");
  EXPECT_EQ(owned[1], "XYZ");
  EXPECT_EQ(owned[2], "123");
}

TEST(ETWProperties, StringAttributeCopiesByViewLengthNotNul)
{
  const char buffer[] = {'h', 'e', 'l', 'l', 'o', '!', '@', '#'};
  common::AttributeValue attr{nostd::string_view(buffer, 5)};

  PropertyValue value;
  value.FromAttributeValue(attr);

  ASSERT_EQ(value.index(), static_cast<size_t>(exporter::etw::kTypeString));
  EXPECT_EQ(nostd::get<std::string>(value), "hello");
}

TEST(ETWProperties, PropertiesMapPreservesSpanStringAttribute)
{
  const nostd::string_view views[]                                         = {"one", "two"};
  std::vector<std::pair<nostd::string_view, common::AttributeValue>> pairs = {
      {"tags", nostd::span<const nostd::string_view>{views}}};
  auto iterable = common::MakeKeyValueIterableView(pairs);

  Properties props(iterable);
  ASSERT_EQ(props.size(), 1u);

  auto it = props.find("tags");
  ASSERT_NE(it, props.end());
  const auto &owned = nostd::get<std::vector<std::string>>(it->second);
  ASSERT_EQ(owned.size(), 2u);
  EXPECT_EQ(owned[0], "one");
  EXPECT_EQ(owned[1], "two");
}
