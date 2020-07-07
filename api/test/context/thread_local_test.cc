
#include "opentelemetry/context/threadlocal_context.h"

#include <gtest/gtest.h>

using namespace opentelemetry;

/* Tests whether the ThreadLocalContext attaches and detaches as expected */
TEST(ThreadLocalContextTest, ThreadLocalContextAttachDetach)
{

  using M = std::map<std::string, common::AttributeValue>;

  context::Context test_context  = context::Context();
  context::Context::Key test_key = test_context.CreateKey("test_key");
  context::Context::Key foo_key  = test_context.CreateKey("test_key");

  M m1 = {{std::string(test_key.GetIdentifier()), "123"},
          {std::string(foo_key.GetIdentifier()), "456"}};

  context::Context foo_context = test_context.WriteValues(m1);

  context::ThreadLocalContext test_thread_context;

  context::ThreadLocalContext::Token test_token = test_thread_context.Attach(foo_context);

  EXPECT_EQ(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)),
            "123");

  EXPECT_EQ(test_thread_context.Detach(test_token), 0);

  EXPECT_NE(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)),
            "123");
}
