#include <cstdint>

#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/list.h"
#include "opentelemetry/nostd/pair.h"

#include <gtest/gtest.h>

using namespace opentelemetry;

// Test ensurs that the context object doe not change when you write to it 
TEST(ContextTest, Common)
{
  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context::Key test_key = context::Context::CreateKey("test_key");
  context::Context::Key foo_key  = context::Context::CreateKey("test_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};

  context::Context test_context = context::Context();
  context::Context foo_context = test_context.WriteValues(m1);

  EXPECT_NE(nostd::get<nostd::string_view>(test_context.GetValue(test_key)), "123");
  EXPECT_NE(nostd::get<nostd::string_view>(test_context.GetValue(foo_key)), "456");
}

// Tests whether that a  Context Object can return a value that was
// written to it.
TEST(ContextTest, ContextGetWrittenValue)
{

  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context test_context = context::Context();

  context::Context::Key test_key  = test_context.CreateKey("test_key");
  context::Context::Key foo_key   = test_context.CreateKey("foo_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};

  context::Context foo_context   = test_context.WriteValues(m1);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(foo_key)), "456");
}

// Tests whether the new Context Objects inherits the keys and values
// of the original context object.
TEST(ContextTest, ContextInheritance)
{

  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context test_context = context::Context();

  context::Context::Key test_key  = test_context.CreateKey("test_key");
  context::Context::Key foo_key   = test_context.CreateKey("foo_key");
  context::Context::Key other_key = test_context.CreateKey("other_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};
  M m2 = {{std::string(other_key.GetIdentifier()), "000"}};

  context::Context foo_context   = test_context.WriteValues(m1);
  context::Context other_context = foo_context.WriteValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(other_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(other_context.GetValue(foo_key)), "456");
}

// Tests that when you add a key value pair where the key is already in
// existance, they key is overwritten.
TEST(ContextTest, ContextKeyOverwrite)
{

  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context test_context = context::Context();

  context::Context::Key test_key  = test_context.CreateKey("test_key");
  context::Context::Key foo_key   = test_context.CreateKey("foo_key");
  context::Context::Key other_key = test_context.CreateKey("other_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};
  M m2 = {{std::string(test_key.GetIdentifier()), "000"}};

  context::Context foo_context   = test_context.WriteValues(m1);
  context::Context other_context = foo_context.WriteValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(other_context.GetValue(test_key)), "000");
  EXPECT_NE(nostd::get<nostd::string_view>(other_context.GetValue(test_key)), "123");
}

// Tests that copying a context copies the key value pairs properly.
TEST(ContextTest, ContextCopy)
{

  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context test_context = context::Context();

  context::Context::Key test_key  = test_context.CreateKey("test_key");
  context::Context::Key foo_key   = test_context.CreateKey("foo_key");
  context::Context::Key other_key = test_context.CreateKey("other_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};
  M m2 = {{std::string(other_key.GetIdentifier()), "000"}};

  context::Context foo_context    = test_context.WriteValues(m1);
  context::Context other_context  = foo_context.WriteValues(m2);
  context::Context copied_context = other_context;

  EXPECT_EQ(nostd::get<nostd::string_view>(copied_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(copied_context.GetValue(foo_key)), "456");
}

// Tests that the comparison compares properly.
TEST(ContextTest, ContextCompare)
{

  using M                          = std::map<std::string, common::AttributeValue>;
  context::Context test_context = context::Context();

  context::Context::Key test_key  = test_context.CreateKey("test_key");
  context::Context::Key foo_key   = test_context.CreateKey("foo_key");
  context::Context::Key other_key = test_context.CreateKey("other_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
    {std::string(foo_key.GetIdentifier()), "456"}};
  M m2 = {{std::string(other_key.GetIdentifier()), "000"}};

  context::Context foo_context    = test_context.WriteValues(m1);
  context::Context other_context  = foo_context.WriteValues(m2);
  context::Context copied_context = other_context;
  
  EXPECT_TRUE(copied_context == other_context);
  EXPECT_FALSE(copied_context == foo_context);
}
