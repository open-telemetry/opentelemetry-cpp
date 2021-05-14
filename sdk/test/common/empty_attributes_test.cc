// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/sdk/common/empty_attributes.h"

#include <gtest/gtest.h>

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
