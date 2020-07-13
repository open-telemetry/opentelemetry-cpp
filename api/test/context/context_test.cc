#include "opentelemetry/context/context.h"

#include <map>

#include <gtest/gtest.h>

using namespace opentelemetry;


// Tests that the context constructor accepts an std::map
TEST(ContextTest, ContextIterableAcceptsMap)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", "123"}};
  context::Context context_test               = context::Context(map_test);
}

// Tests that the GetValue method returns the expected value
TEST(ContextTest, ContextGetValueReturnsExpectedValue)
{
  std::map<std::string, std::string> map_test       = {{"test_key", "123"},{"foo_key", "456"}};
  
  context::Context context_test                     = context::Context(map_test);
  EXPECT_EQ(nostd::get<nostd::string_view>(context_test.GetValue("test_key")), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(context_test.GetValue("foo_key")), "456");
}

// Tests that the SetValues method accepts an std::map
TEST(ContextTest, ContextSetValuesAcceptsMap)
{
  std::map<std::string, context::ContextValue> map_test       = {{"test_key", "123"}};
  std::map<std::string, context::ContextValue> map_test_write = {{"foo_key", "456"}};
  context::Context context_test                     = context::Context(map_test);
  context::Context foo_test = context_test.SetValues(map_test_write);
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_test.GetValue("test_key")), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_test.GetValue("foo_key")), "456");
}

// Tests that the SetValues method accepts a nostd::string_view and 
// context::ContextValue
TEST(ContextTest, ContextSetValuesAcceptsStringViewContextValue)
{
  nostd::string_view string_view_test = "string_view";
  context::ContextValue context_value_test = "123";
  context::Context context_test                     = context::Context();
  context::Context foo_test = context_test.SetValue(string_view_test, context_value_test);
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_test.GetValue(string_view_test)), "123");
}

// Tests that the original context does not change when a value is 
// written to it
TEST(ContextTest, ContextImmutability)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", "123"}};
  context::Context context_test               = context::Context(map_test);

  context::Context context_foo               = context_test.SetValue("foo_key", "456");
  
  EXPECT_THROW(nostd::get<nostd::string_view>(context_test.GetValue("foo_key")), nostd::bad_variant_access);
}
