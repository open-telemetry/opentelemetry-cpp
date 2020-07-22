#include "opentelemetry/context/threadlocal_context.h"

#include <gtest/gtest.h>

using namespace opentelemetry;

// Tests that GetCurrent returns the current context
TEST(ThreadLocalContextTest, ThreadLocalGetCurrent)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::ThreadLocalContext::Token old_context =
      context::ThreadLocalContext::Attach(&test_context);
  EXPECT_TRUE(*context::ThreadLocalContext::GetCurrent() == test_context);
  context::ThreadLocalContext::Detach(old_context);
}

// Tests that detach resets the context to the previous context
TEST(ThreadLocalContextTest, ThreadLocalDetach)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);
  context::ThreadLocalContext::Token test_context_token =
      context::ThreadLocalContext::Attach(&test_context);
  context::ThreadLocalContext::Token foo_context_token =
      context::ThreadLocalContext::Attach(&foo_context);
  context::ThreadLocalContext::Detach(foo_context_token);
  EXPECT_TRUE(*context::ThreadLocalContext::GetCurrent() == test_context);
  context::ThreadLocalContext::Detach(test_context_token);
}

// Tests that detach returns false when the wrong context is provided
TEST(ThreadLocalContextTest, ThreadLocalDetachWrongContext)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);
  context::ThreadLocalContext::Token test_context_token =
      context::ThreadLocalContext::Attach(&test_context);
  context::ThreadLocalContext::Token foo_context_token =
      context::ThreadLocalContext::Attach(&foo_context);
  EXPECT_FALSE(context::ThreadLocalContext::Detach(test_context_token));
  context::ThreadLocalContext::Detach(foo_context_token);
  context::ThreadLocalContext::Detach(test_context_token);
}

// Tests that the ThreadLocalContext can handle three attached contexts
TEST(ThreadLocalContextTest, ThreadLocalThreeAttachDetach)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);
  context::Context other_context                        = context::Context(map_test);
  context::ThreadLocalContext::Token test_context_token =
      context::ThreadLocalContext::Attach(&test_context);
  context::ThreadLocalContext::Token foo_context_token =
      context::ThreadLocalContext::Attach(&foo_context);
  context::ThreadLocalContext::Token other_context_token =
      context::ThreadLocalContext::Attach(&other_context);

  EXPECT_TRUE(context::ThreadLocalContext::Detach(other_context_token));
  EXPECT_TRUE(context::ThreadLocalContext::Detach(foo_context_token));
  EXPECT_TRUE(context::ThreadLocalContext::Detach(test_context_token));
}
