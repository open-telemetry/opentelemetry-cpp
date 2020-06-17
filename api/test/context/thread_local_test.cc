
#include "opentelemetry/context/threadlocal_context.h"

#include <gtest/gtest.h>

using namespace opentelemetry;

TEST(Thread_Local_Context_Test, thread_local_context){

  context::Key test_key = context::Key("test_key");
  context::Key other_key = context::Key("other_key");
  context::Key foo_key = context::Key("foo_key");

  using M = std::map<context::Key*, std::string>;

  M m1    = {{&test_key, "123"}, {&other_key, "456"}};
  M m2    = {{&foo_key, "000"}};

  context::KeyValueIterableModifiable<M> iterable_1{m1};

  context::Context<M> test_context = context::Context<M>(iterable_1);
  context::ThreadLocalContext<M> test_thread_context; 

  context::ThreadLocalContext<M>::Token test_token = test_thread_context.Attach(test_context);
  
  EXPECT_EQ(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)), "123");

  EXPECT_EQ(test_thread_context.Detach(test_token), 0);
  EXPECT_NE(nostd::get<nostd::string_view>(test_thread_context.GetCurrent().GetValue(test_key)), "123");
}
