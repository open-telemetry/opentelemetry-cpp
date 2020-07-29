#include "opentelemetry/context/context.h"
#include "opentelemetry/context/threadlocal_context.h"

#include <gtest/gtest.h>

using namespace opentelemetry;

// Tests that GetCurrent returns the current context
TEST(RuntimeContextTest, GetCurrent)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::RuntimeContext::Token old_context = context::RuntimeContext::Attach(test_context);
  EXPECT_TRUE(context::RuntimeContext::GetCurrent() == test_context);
  context::RuntimeContext::Detach(old_context);
}

// Tests that detach resets the context to the previous context
TEST(RuntimeContextTest, Detach)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);

  context::RuntimeContext::Token test_context_token = context::RuntimeContext::Attach(test_context);
  context::RuntimeContext::Token foo_context_token  = context::RuntimeContext::Attach(foo_context);

  context::RuntimeContext::Detach(foo_context_token);
  EXPECT_TRUE(context::RuntimeContext::GetCurrent() == test_context);
  context::RuntimeContext::Detach(test_context_token);
}

// Tests that detach returns false when the wrong context is provided
TEST(RuntimeContextTest, DetachWrongContext)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);
  context::RuntimeContext::Token test_context_token = context::RuntimeContext::Attach(test_context);
  context::RuntimeContext::Token foo_context_token  = context::RuntimeContext::Attach(foo_context);
  EXPECT_FALSE(context::RuntimeContext::Detach(test_context_token));
  context::RuntimeContext::Detach(foo_context_token);
  context::RuntimeContext::Detach(test_context_token);
}

// Tests that the ThreadLocalContext can handle three attached contexts
TEST(RuntimeContextTest, ThreeAttachDetach)
{
  std::map<std::string, context::ContextValue> map_test = {{"test_key", (int64_t)123}};
  context::Context test_context                         = context::Context(map_test);
  context::Context foo_context                          = context::Context(map_test);
  context::Context other_context                        = context::Context(map_test);
  context::RuntimeContext::Token test_context_token = context::RuntimeContext::Attach(test_context);
  context::RuntimeContext::Token foo_context_token  = context::RuntimeContext::Attach(foo_context);
  context::RuntimeContext::Token other_context_token =
      context::RuntimeContext::Attach(other_context);

  EXPECT_TRUE(context::RuntimeContext::Detach(other_context_token));
  EXPECT_TRUE(context::RuntimeContext::Detach(foo_context_token));
  EXPECT_TRUE(context::RuntimeContext::Detach(test_context_token));
}
