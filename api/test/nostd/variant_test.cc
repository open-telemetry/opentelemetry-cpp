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

TEST(VariantTest, Get) {
  nostd::variant<int, float> v, w;
  v = 12;
  EXPECT_EQ(nostd::get<int>(v), 12);
  EXPECT_EQ(nostd::get<0>(v), 12);
  w = v;
  EXPECT_EQ(nostd::get<int>(w), 12);
  EXPECT_EQ(*nostd::get_if<int>(&v), 12);
  EXPECT_EQ(nostd::get_if<float>(&v), nullptr);
#if __EXCEPTIONS
  EXPECT_THROW(nostd::get<float>(w), nostd::bad_variant_access);
#else
  EXPECT_DEATH({ nostd::get<float>(w); }, "");
#endif
}

TEST(VariantTest, Comparison) {
  nostd::variant<int, float> v, w;
  EXPECT_TRUE(v == w);
  EXPECT_FALSE(v != w);
  v = 3.0f;
  EXPECT_TRUE(v != w);
  EXPECT_FALSE(v == w);
}

TEST(VariantTest, Visit) {
  nostd::variant<int, float> v;
  struct {
    int operator()(int) { return 0; }
    int operator()(float) { return 1; }
  } a;
  EXPECT_EQ(nostd::visit(a, v), 0);
  v = 2.0f;
  EXPECT_EQ(nostd::visit(a, v), 1);
}
