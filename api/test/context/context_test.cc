#include "opentelemetry/context/context.h"

#include <gtest/gtest.h>

using namespace opentelemetry::context;

/* Tests whether the original context objects changes 
 * when you write to it.
 */
TEST(Context_test, is_context_immutable)
{

  Context test_context = Context();

  Context::ContextKey test_key = test_context.CreateKey("test_key");
 
  EXPECT_EQ(test_context.GetValue(test_key), NULL);

  Context new_test_context = test_context.WriteValue(test_key,7);

  EXPECT_EQ(new_test_context.GetValue(test_key), 7);

  EXPECT_EQ(test_context.GetValue(test_key), NULL);
}

/* Tests whether the new Context Objects inherits the keys and values
 * of the original context object 
 */
TEST(Context_test, context_write_new_object)
{


  Context::ContextKey test_key = Context::ContextKey("test_key");

  Context test_context = Context(test_key, 7);
  
  Context::ContextKey foo_key = test_context.CreateKey("foo_key");

  Context new_test_context = test_context.WriteValue(foo_key,1);
 
  EXPECT_EQ(new_test_context.GetValue(test_key), 7);

  EXPECT_EQ(new_test_context.GetValue(foo_key), 1);
}





