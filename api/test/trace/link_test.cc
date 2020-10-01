#include "opentelemetry/trace/link.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/span_context.h"

#include <gtest/gtest.h>
#include <iostream>
#include <map>

using namespace opentelemetry;

using opentelemetry::trace::KeyValueIterable;
using opentelemetry::trace::KeyValueIterableView;
using opentelemetry::trace::Link;
using opentelemetry::trace::SpanContext;

using Map = std::map<nostd::string_view, nostd::string_view>;
void validateKeyValueIterableView(const KeyValueIterable &attributes, Map &keys)
{
  attributes.ForEachKeyValue([&](nostd::string_view key,
                                 opentelemetry::common::AttributeValue value) noexcept {
    bool found = (keys.end() != keys.find(key));
    EXPECT_EQ(found, true);
    return true;
  });
}

void validateKeyValueInitializationList(const KeyValueIterable &attributes,
                                        const std::vector<nostd::string_view> &keys)
{
  int i = 0;
  attributes.ForEachKeyValue([&](nostd::string_view key,
                                 opentelemetry::common::AttributeValue value) noexcept {
    std::cout << key;
    return true;
  });
}
TEST(LinkTest, CreateLinkIterableView)
{

  Map m1 = {{"abc", "123"}, {"xyz", "456"}};
  SpanContext s1(true, true);
  KeyValueIterableView<Map> iterable{m1};

  Link link(s1, iterable);
  validateKeyValueIterableView(link.GetAttributes(), m1);
  EXPECT_EQ(s1.IsSampled(), link.GetSpanContext().IsSampled());
}

TEST(LinkTest, CreateLinkInitlilizerList)
{

  SpanContext s1(true, true);
  Link link1(s1, {{"k1", "v1"}, {"k2", "v2"}});

  std::map<std::string, int> attributes = {{"attr1", 1}, {"attr2", 2}};
  Link link2(s1, attributes);
}
