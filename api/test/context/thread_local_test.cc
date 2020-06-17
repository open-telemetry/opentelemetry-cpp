/*
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/context/key_value_iterable_modifiable.h"
 */

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/threadlocal_context.h"

#include <gtest/gtest.h>


using namespace opentelemetry/*::Context*/;

TEST(ThreadLocalContextTest, GetThreadlocalContext)
{

  using M = std::map< context::Key* ,  nostd::string_view>;

  context::Key test_key = context::Key("test_key");

  M m1    = {{&test_key, "123"}};


//  std::cout << "Hello World" << std::endl;

//  context::ThreadLocalContext<M> tlc;

}

