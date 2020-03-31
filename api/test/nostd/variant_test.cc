#include "opentelemetry/nostd/variant.h"

#include <type_traits>

#include <gtest/gtest.h>

/* using opentelemetry::nostd::variant; */

TEST(TypePackElementTest, IndexedType) {
  using opentelemetry::nostd::detail::type_pack_element_t;
  EXPECT_TRUE((std::is_same<type_pack_element_t<0, int, double, char>, int>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<1, int, double, char>, double>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<2, int, double, char>, char>::value));
}
