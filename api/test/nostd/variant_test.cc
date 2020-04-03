#include "opentelemetry/nostd/variant.h"

#include <type_traits>

#include <gtest/gtest.h>

namespace nostd = opentelemetry::nostd;
/* using opentelemetry::nostd::variant; */
/* using opentelemetry::nostd::variant_alternative_t; */
/* using opentelemetry::nostd::variant_size; */

TEST(TypePackElementTest, IndexedType)
{
  using opentelemetry::nostd::detail::type_pack_element_t;
  EXPECT_TRUE((std::is_same<type_pack_element_t<0, int, double, char>, int>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<1, int, double, char>, double>::value));
  EXPECT_TRUE((std::is_same<type_pack_element_t<2, int, double, char>, char>::value));
}

TEST(VariantSizeTest, GetVariantSize)
{
  EXPECT_EQ(nostd::variant_size<nostd::variant<>>::value, 0);
  EXPECT_EQ(nostd::variant_size<nostd::variant<int>>::value, 1);
  EXPECT_EQ((nostd::variant_size<nostd::variant<int, double>>::value), 2);
}

TEST(VariantAlternativeTest, GetVariantSize)
{
  EXPECT_TRUE((std::is_same<nostd::variant_alternative_t<0, nostd::variant<int>>, int>::value));
  EXPECT_TRUE((std::is_same<nostd::variant_alternative_t<1, nostd::variant<int, double>>, double>::value));
  EXPECT_TRUE(
      (std::is_same<nostd::variant_alternative_t<1, const nostd::variant<int, double>>, const double>::value));
}

struct A {
  int operator()(int) { return 1;}

  char operator()(char) { return 'a';}
};

TEST(VariantTest, Get) {
  nostd::variant<int, float> v, w;
  v = 12;
  EXPECT_EQ(nostd::get<int>(v), 12);
}
