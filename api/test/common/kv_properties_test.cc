
#include <gtest/gtest.h>
#include <opentelemetry/common/kv_properties.h>

// ------------------------- Entry class tests ---------------------------------

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