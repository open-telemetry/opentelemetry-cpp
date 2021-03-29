
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

#include <gtest/gtest.h>
#include <opentelemetry/common/kv_properties.h>

#include <string>
#include <utility>
#include <vector>

// ------------------------- Entry class tests ---------------------------------

using namespace opentelemetry;
using opentelemetry::common::KeyValueProperties;
// Test constructor that takes a key-value pair
TEST(EntryTest, KeyValueConstruction)
{
  opentelemetry::nostd::string_view key = "test_key";
  opentelemetry::nostd::string_view val = "test_value";
  KeyValueProperties::Entry e(key, val);

  EXPECT_EQ(key.size(), e.GetKey().size());
  EXPECT_EQ(key, e.GetKey());

  EXPECT_EQ(val.size(), e.GetValue().size());
  EXPECT_EQ(val, e.GetValue());
}

// Test copy constructor
TEST(EntryTest, Copy)
{
  KeyValueProperties::Entry e("test_key", "test_value");
  KeyValueProperties::Entry copy(e);
  EXPECT_EQ(copy.GetKey(), e.GetKey());
  EXPECT_EQ(copy.GetValue(), e.GetValue());
}

// Test assignment operator
TEST(EntryTest, Assignment)
{
  KeyValueProperties::Entry e("test_key", "test_value");
  KeyValueProperties::Entry empty;
  empty = e;
  EXPECT_EQ(empty.GetKey(), e.GetKey());
  EXPECT_EQ(empty.GetValue(), e.GetValue());
}

TEST(EntryTest, SetValue)
{
  KeyValueProperties::Entry e("test_key", "test_value");
  opentelemetry::nostd::string_view new_val = "new_value";
  e.SetValue(new_val);

  EXPECT_EQ(new_val.size(), e.GetValue().size());
  EXPECT_EQ(new_val, e.GetValue());
}

// ------------------------- KeyValueStringIterator tests ---------------------------------

using opentelemetry::common::KeyValueStringIterator;

TEST(KVStringIter, SinglePair)
{
  bool valid_kv;
  nostd::string_view key, value;
  opentelemetry::nostd::string_view str = "k1=v1";
  KeyValueStringIterator itr(str, ',', '=');
  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_TRUE(valid_kv);
  EXPECT_EQ(key, "k1");
  EXPECT_EQ(value, "v1");
  EXPECT_FALSE(itr.next(valid_kv, key, value));
}

TEST(KVStringIter, ValidPairsWithEmptyEntries)
{
  opentelemetry::nostd::string_view str = "k1:v1===k2:v2==";
  bool valid_kv;
  nostd::string_view key, value;
  KeyValueStringIterator itr(str, '=', ':');
  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_TRUE(valid_kv);
  EXPECT_EQ(key, "k1");
  EXPECT_EQ(value, "v1");

  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_TRUE(valid_kv);
  EXPECT_EQ(key, "k2");
  EXPECT_EQ(value, "v2");

  EXPECT_FALSE(itr.next(valid_kv, key, value));
}

TEST(KVStringIter, InvalidPairs)
{
  opentelemetry::nostd::string_view str = "k1=v1,invalid  ,,  k2=v2   ,invalid";
  KeyValueStringIterator itr(str);
  bool valid_kv;
  nostd::string_view key, value;
  EXPECT_TRUE(itr.next(valid_kv, key, value));

  EXPECT_TRUE(valid_kv);
  EXPECT_EQ(key, "k1");
  EXPECT_EQ(value, "v1");

  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_FALSE(valid_kv);

  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_TRUE(valid_kv);
  EXPECT_EQ(key, "k2");
  EXPECT_EQ(value, "v2");

  EXPECT_TRUE(itr.next(valid_kv, key, value));
  EXPECT_FALSE(valid_kv);

  EXPECT_FALSE(itr.next(valid_kv, key, value));
}

//------------------------- KeyValueProperties tests ---------------------------------

TEST(KeyValueProperties, PopulateKVIterableContainer)
{
  std::vector<std::pair<std::string, std::string>> kv_pairs = {{"k1", "v1"}, {"k2", "v2"}};

  auto kv_properties = KeyValueProperties(kv_pairs);
  EXPECT_EQ(kv_properties.Size(), 2);

  std::string value;
  bool present = kv_properties.GetValue("k1", value);
  EXPECT_TRUE(present);
  EXPECT_EQ(value, "v1");

  present = kv_properties.GetValue("k2", value);
  EXPECT_TRUE(present);
  EXPECT_EQ(value, "v2");
}

TEST(KeyValueProperties, AddEntry)
{
  auto kv_properties = KeyValueProperties(1);
  kv_properties.AddEntry("k1", "v1");
  std::string value;
  bool present = kv_properties.GetValue("k1", value);
  EXPECT_TRUE(present);
  EXPECT_EQ(value, "v1");

  kv_properties.AddEntry("k2", "v2");  // entry will not be added as max size reached.
  EXPECT_EQ(kv_properties.Size(), 1);
  present = kv_properties.GetValue("k2", value);
  EXPECT_FALSE(present);
}

TEST(KeyValueProperties, GetValue)
{
  auto kv_properties = KeyValueProperties(1);
  kv_properties.AddEntry("k1", "v1");
  std::string value;
  bool present = kv_properties.GetValue("k1", value);
  EXPECT_TRUE(present);
  EXPECT_EQ(value, "v1");

  present = kv_properties.GetValue("k3", value);
  EXPECT_FALSE(present);
}

TEST(KeyValueProperties, GetAllEntries)
{
  std::vector<std::pair<std::string, std::string>> kv_pairs = {
      {"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
  const size_t kNumPairs                              = 3;
  opentelemetry::nostd::string_view keys[kNumPairs]   = {"k1", "k2", "k3"};
  opentelemetry::nostd::string_view values[kNumPairs] = {"v1", "v2", "v3"};
  auto kv_properties                                  = KeyValueProperties(kv_pairs);

  size_t index = 0;
  kv_properties.GetAllEntries(
      [&keys, &values, &index](nostd::string_view key, nostd::string_view value) {
        EXPECT_EQ(key, keys[index]);
        EXPECT_EQ(value, values[index]);
        index++;
        return true;
      });

  EXPECT_EQ(index, kNumPairs);
}
