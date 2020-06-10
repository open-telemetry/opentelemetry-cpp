#include "opentelemetry/context/context.h"

#include <gtest/gtest.h>

using namespace opentelemetry::context;


/* Tests whether the runtimeContext object properly returns the current context
 */
TEST(runtimeContext_test, get_current_context)
{

  Context::ContextKey test_key = Context::ContextKey("test_key");
  Context test_context = Context(test_key, 7);
  
  RuntimeContext test_runtime = RuntimeContext(test_context);

  EXPECT_EQ(test_runtime.GetCurrent().GetValue(test_key), 
            test_context.GetValue(test_key));  

}




/* Tests whether the runtimeContext object properly attaches and detaches
 * the context object. 
 */ 
TEST(runtimeContext_test, attach_detach_context)
{ 
 
  Context::ContextKey test_key = Context::ContextKey("test_key");
  Context test_context = Context(test_key, 7);
  
  RuntimeContext test_runtime = RuntimeContext(test_context);
   
  Context::ContextKey foo_key = Context::ContextKey("foo_key");
  Context foo_context = Context(foo_key, 5);

  EXPECT_EQ(test_runtime.GetCurrent().GetValue(test_key), 
            test_context.GetValue(test_key));  
  EXPECT_NE(test_runtime.GetCurrent().GetValue(foo_key), 
            foo_context.GetValue(foo_key));  

  Token test_token = test_runtime.Attach(foo_context);

  EXPECT_NE(test_runtime.GetCurrent().GetValue(test_key), 
            test_context.GetValue(test_key));  
  EXPECT_EQ(test_runtime.GetCurrent().GetValue(foo_key), 
            foo_context.GetValue(foo_key));  

  int detach_result = test_runtime.Detach(test_token);  

  EXPECT_EQ(detach_result, 0);  
  
  EXPECT_EQ(test_runtime.GetCurrent().GetValue(test_key), 
            test_context.GetValue(test_key));  
  EXPECT_NE(test_runtime.GetCurrent().GetValue(foo_key), 
            foo_context.GetValue(foo_key));  
}

