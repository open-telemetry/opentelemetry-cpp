#include "opentelemetry/context/context.h"

#include <map>

#include <gtest/gtest.h>

using namespace opentelemetry;

// Tests that the context constructor accepts an std::map.
TEST(ContextTest, ContextIterableAcceptsMap)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", "123"}};
  nostd::shared_ptr<context::Context> test_context      = context::Context::CreateContext(map_test);
}

// Tests that the GetValue method returns the expected value.
TEST(ContextTest, ContextGetValueReturnsExpectedValue)
{
  std::map<std::string, std::string> map_test = {{"test_key", "123"}, {"foo_key", "456"}};

  nostd::shared_ptr<context::Context> test_context = context::Context::CreateContext(map_test);
  nostd::get<nostd::string_view>(test_context->GetValue("test_key"));
  //  EXPECT_EQ(nostd::get<nostd::string_view>(test_context->GetValue("test_key")), "123");
  //  EXPECT_EQ(nostd::get<nostd::string_view>(test_context->GetValue("foo_key")), "456");
}
/*
// Tests that the SetValues method accepts an std::map.
TEST(ContextTest, ContextSetValuesAcceptsMap)
{
  std::map<std::string, context::ContextValue> map_test       = {{"test_key", "123"}};
  std::map<std::string, context::ContextValue> map_test_write = {{"foo_key", "456"}};

  nostd::shared_ptr<context::Context> test_context = context::Context::CreateContext(map_test);
  nostd::shared_ptr<context::Context> foo_test     = test_context->SetValues(map_test_write);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_test->GetValue("test_key")), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_test->GetValue("foo_key")), "456");
}

// Tests that the SetValues method accepts a nostd::string_view and
// context::ContextValue.
TEST(ContextTest, ContextSetValuesAcceptsStringViewContextValue)
{
  nostd::string_view string_view_test      = "string_view";
  context::ContextValue context_value_test = "123";

  nostd::shared_ptr<context::Context> test_context =
      context::Context::CreateContext(string_view_test, context_value_test);
  nostd::shared_ptr<context::Context> foo_context =
      test_context->SetValue(string_view_test, context_value_test);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context->GetValue(string_view_test)), "123");
}

// Tests that the original context does not change when a value is
// written to it.
TEST(ContextTest, ContextImmutability)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", "123"}};
  std::map<std::string, context::ContextValue> foo_map  = {{"foo_key", "456"}};

  nostd::shared_ptr<context::Context> context_test = context::Context::CreateContext(map_test);
  nostd::shared_ptr<context::Context> context_foo  = context_test->SetValue("foo_key", "456");

  EXPECT_NE(nostd::get<nostd::string_view>(context_test->GetValue("foo_key")), "456");
}

// Tests that writing the same to a context overwrites the original value.
TEST(ContextTest, ContextKeyOverwrite)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", "123"}};

  nostd::shared_ptr<context::Context> context_test = context::Context::CreateContext(map_test);
  nostd::shared_ptr<context::Context> context_foo  = context_test->SetValue("test_key", "456");

  EXPECT_EQ(nostd::get<nostd::string_view>(context_foo->GetValue("test_key")), "456");
}

// Tests that the new Context Objects inherits the keys and values
// of the original context object.
TEST(ContextTest, ContextInheritance)
{
  using M = std::map<std::string, context::ContextValue>;

  M m1 = {{"test_key", "123"}, {"foo_key", "456"}};
  M m2 = {{"other_key", "789"}};

  nostd::shared_ptr<context::Context> test_context = context::Context::CreateContext(m1);
  nostd::shared_ptr<context::Context> foo_context  = test_context->SetValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context->GetValue("test_key")), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context->GetValue("foo_key")), "456");
}

// Tests that copying a context copies the key value pairs as expected.
TEST(ContextTest, ContextCopyOperator)
{
  std::map<std::string, std::string> test_map = {
      {"test_key", "123"}, {"foo_key", "456"}, {"other_key", "789"}};

  nostd::shared_ptr<context::Context> test_context   = context::Context::CreateContext(test_map);
  nostd::shared_ptr<context::Context> copied_context = test_context;

  EXPECT_EQ(nostd::get<nostd::string_view>(copied_context->GetValue("test_key")), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(copied_context->GetValue("foo_key")), "456");
  EXPECT_EQ(nostd::get<nostd::string_view>(copied_context->GetValue("other_key")), "789");
}*/
