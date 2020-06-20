
#include <cstdint>

#include "opentelemetry/context/context.h"

#include <gtest/gtest.h>


using namespace opentelemetry;

/* Test ensurs that the context object doe not change when you write to it */
TEST(Context_test, is_context_immutable)
{
  using M = std::map< std::string, common::AttributeValue>;
  context::Context<M> test_context = context::Context<M>();

  context::Context<M>::Key test_key = test_context.CreateKey("test_key"); 
  context::Context<M>::Key foo_key = test_context.CreateKey("test_key"); 

  M m1    = {{std::string(test_key.GetIdentifier()) , "123"}, {std::string(foo_key.GetIdentifier()) , "456"}};

  context::Context<M> foo_context = test_context.WriteValues(m1);

  EXPECT_NE(nostd::get<nostd::string_view>(test_context.GetValue(test_key)), "123");
  EXPECT_NE(nostd::get<nostd::string_view>(test_context.GetValue(foo_key)), "456");
}

/* Tests whether the new Context Objects inherits the keys and values
 * of the original context object 
 */
TEST(Context_test, context_write_new_object){

  using M = std::map< std::string, common::AttributeValue>;
  context::Context<M> test_context = context::Context<M>();

  context::Context<M>::Key test_key = test_context.CreateKey("test_key"); 
  context::Context<M>::Key foo_key = test_context.CreateKey("foo_key"); 
  context::Context<M>::Key other_key = test_context.CreateKey("other_key"); 

  M m1    = {{std::string(test_key.GetIdentifier()) , "123"}, {std::string(foo_key.GetIdentifier()) , "456"}};
  M m2    = {{std::string(other_key.GetIdentifier()) , "000"}};

  context::Context<M> foo_context = test_context.WriteValues(m1);
  context::Context<M> other_context = foo_context.WriteValues(m2);

  EXPECT_EQ(nostd::get<nostd::string_view>(other_context.GetValue(test_key)), "123");
  EXPECT_EQ(nostd::get<nostd::string_view>(other_context.GetValue(foo_key)), "456");
}
/*
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
 */
