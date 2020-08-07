#include "opentelemetry/sdk/common/empty_attributes.h"

#include <gtest/gtest.h>

#if 0

TEST(EmptyAttributesTest, TestSize)
{
  EXPECT_EQ(opentelemetry::sdk::GetEmptyAttributes().size(), 0);
}

// Test that GetEmptyAttributes() always returns the same KeyValueIterableView
TEST(EmptyAttributesTest, TestMemory)
{
  auto attributes1 = opentelemetry::sdk::GetEmptyAttributes();
  auto attributes2 = opentelemetry::sdk::GetEmptyAttributes();
  EXPECT_EQ(memcmp(&attributes1, &attributes2, sizeof(attributes1)), 0);
}

#endif
