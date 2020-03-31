#include "opentelemetry/nostd/variant.h"

#include <type_traits>

#include <gtest/gtest.h>

using opentelemetry::nostd::variant;
using opentelemetry::nostd::variant_size;

TEST(TypePackElementTest, IndexedType) {
  using opentelemetry::nostd::detail::type_pack_element_t;
  EXPECT_TRUE((std::is_same<type_pack_element_t<0, int, double, char>, int>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<1, int, double, char>, double>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<2, int, double, char>, char>::value));
}

TEST(VariantSizeTest, GetVariantSize) {
  EXPECT_EQ(variant_size<variant<>>::value, 0); 
  EXPECT_EQ(variant_size<variant<int>>::value, 1); 
  EXPECT_EQ((variant_size<variant<int, double>>::value), 2); 
}
