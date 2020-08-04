#include "opentelemetry/sdk/trace/attribute_utils.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

#include <gtest/gtest.h>

TEST(AttributeMapTest, DefaultContruction)
{
  opentelemetry::sdk::trace::AttributeMap map;
  EXPECT_EQ(map.GetAttributes().size(), 0);
}

TEST(AttributeMapTest, AttributesContruction)
{
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {4, 12, 33};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  opentelemetry::trace::KeyValueIterableView<std::map<std::string, int>> iterable(attributes);
  opentelemetry::sdk::trace::AttributeMap map(iterable);

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int64_t>(map.GetAttributes().at(keys[i])), values[i]);
  }
}
