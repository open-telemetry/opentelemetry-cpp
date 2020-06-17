#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/context/key_value_iterable_modifiable.h"

#include <gtest/gtest.h>


  using namespace opentelemetry/*::Context*/;
  //using opentelemetry::context::RuntimeContext;

  /* Tests whether the runtimeContext object properly returns the current context
   */
  TEST(RuntimeContextTest, GetCurrentContext)
  {
    using M = std::map< context::Key* ,  nostd::string_view>;

    context::Key test_key = context::Key("test_key");
    
    M m1    = {{&test_key, "123"}};
    
    context::KeyValueIterableModifiable<M> iterable_1{m1};
  
    context::Context<M> test_context = context::Context<M>(iterable_1);
    
    context::RuntimeContext<M> test_runtime;// = context::RuntimeContext<M>(test_context);
    
    test_runtime.Attach(test_context);
    
    test_context.GetValue(test_key);
    
    test_runtime.GetCurrent().GetValue(test_key);
    
    EXPECT_EQ(nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(test_key)), "123");
    EXPECT_EQ( nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(test_key)), nostd::get<nostd::string_view>(test_context.GetValue(test_key)) );
  }

  /* Tests whether the runtimeContext object properly attaches and detaches
   * the context object.
   */
  TEST(RuntimeContextTest, AttachDetachContext)
  {
  
    using M = std::map< context::Key* ,  nostd::string_view>;

    context::Key test_key = context::Key("test_key");
    
    context::Key foo_key = context::Key("foo_key");
    
    M m1    = {{&test_key, "123"}};
    
    M m2    = {{&foo_key, "534"}};
    
    context::KeyValueIterableModifiable<M> iterable_1{m1};
  
    context::Context<M> test_context = context::Context<M>(iterable_1);
   
    context::KeyValueIterableModifiable<M> iterable_2{m2};
  
    context::Context<M> foo_context = context::Context<M>(iterable_2);
    
    context::RuntimeContext<M> test_runtime;// = context::RuntimeContext<M>(test_context);
    
    test_runtime.Attach(test_context);
     
    EXPECT_EQ(nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(test_key)), "123");
    EXPECT_EQ( nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(test_key)), nostd::get<nostd::string_view>(test_context.GetValue(test_key)) );
  
    context::RuntimeContext<M>::Token old_context = test_runtime.Attach(foo_context);
    
    EXPECT_EQ( nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(foo_key)), nostd::get<nostd::string_view>(foo_context.GetValue(foo_key)) );
    
    test_runtime.Detach(old_context);
    
    EXPECT_EQ( nostd::get<nostd::string_view>(test_runtime.GetCurrent().GetValue(test_key)), nostd::get<nostd::string_view>(test_context.GetValue(test_key)) );
  }
