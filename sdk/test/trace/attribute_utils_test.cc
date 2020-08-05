#include "opentelemetry/sdk/trace/attribute_utils.h"

#include <gtest/gtest.h>

TEST(AttributeMapTest, DefaultConstruction)
{
  opentelemetry::sdk::trace::AttributeMap map;
  EXPECT_EQ(map.GetAttributes().size(), 0);
}

TEST(AttributeMapTest, AttributesConstruction)
{
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {15, 24, 37};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  opentelemetry::trace::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);
  opentelemetry::sdk::trace::AttributeMap map(iterable);

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int64_t>(map.GetAttributes().at(keys[i])), values[i]);
  }
}
