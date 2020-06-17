
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/context/key_value_iterable_modifiable.h"
#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/common/attribute_value.h"

#include <gtest/gtest.h>


using namespace opentelemetry;

/* Tests whether the original context objects changes 
 * when you write to it.
 */
TEST(Context_test, is_context_immutable)
{
  context::Key test_key = context::Key("test_key");
  context::Key other_key = context::Key("other_key");
  context::Key foo_key = context::Key("foo_key");
  
  using M = std::map<context::Key*, common::AttributeValue>;
  
  M m1    = {{&test_key, "123"}, {&other_key, "456"}};
  M m2    = {{&foo_key, "000"}};

  //trace::KeyValueIterable iterable{m1}; 
  //trace::KeyValueIterableView<M> iterable_1{m1};
  
  context::Context<M> test_context = context::Context<M>(m1);
  
  EXPECT_EQ(nostd::get<nostd::string_view>(test_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(test_context.GetValue(other_key)), "456");
  
  context::Context<M> foo_context = test_context.WriteValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(foo_key)), "000");
  EXPECT_NE(nostd::get<nostd::string_view>(test_context.GetValue(foo_key)), "000");

}

/* Tests whether the new Context Objects inherits the keys and values
 * of the original context object 
 */
TEST(Context_test, context_write_new_object){

  context::Key test_key = context::Key("test_key");
  context::Key other_key = context::Key("other_key");
  context::Key foo_key = context::Key("foo_key");
  
  using M = std::map<context::Key*, std::string>;
  
  M m1    = {{&test_key, "123"}, {&other_key, "456"}};
  M m2    = {{&foo_key, "000"}};

  
  context::KeyValueIterableModifiable<M> iterable_1{m1};
  
  context::Context<M> test_context = context::Context<M>(iterable_1);
  
  context::Context<M> foo_context = test_context.WriteValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(other_key)), "456");
  EXPECT_EQ(nostd::get<nostd::string_view>(foo_context.GetValue(foo_key)), "000");
}

TEST(Context_test, thread_local_context){
  
  context::Key test_key = context::Key("test_key");
  context::Key other_key = context::Key("other_key");
  context::Key foo_key = context::Key("foo_key");
  
  using M = std::map<context::Key*, std::string>;
  
  M m1    = {{&test_key, "123"}, {&other_key, "456"}};
  M m2    = {{&foo_key, "000"}};

  
  context::KeyValueIterableModifiable<M> iterable_1{m1};
  
  context::Context<M> test_context = context::Context<M>(iterable_1);
  context::ThreadLocalContext<M> test_thread_context; 
  
  context::Token<M> test_token = test_thread_context.Attach(test_context);

  EXPECT_EQ(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)), "123");
  
  EXPECT_EQ(test_thread_context.Detach(test_token), 0);

  EXPECT_NE(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)), "123");

}
