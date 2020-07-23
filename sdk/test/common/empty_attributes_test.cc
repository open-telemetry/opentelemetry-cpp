#include "opentelemetry/sdk/common/empty_attributes.h"

#include <gtest/gtest.h>

TEST(EmptyAttributesTest, EmptyTest)
{
  EXPECT_EQ(opentelemetry::sdk::GetEmptyAttributes().size(), 0);
}
