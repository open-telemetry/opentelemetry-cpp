// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/nostd/variant.h"

#include <string>
#include <type_traits>

#include <gtest/gtest.h>

namespace nostd = opentelemetry::nostd;

class DestroyCounter
{
public:
  explicit DestroyCounter(int *count) : count_{count} {}
  ~DestroyCounter() { ++*count_; }

private:
  int *count_;
};

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
  EXPECT_TRUE(
      (std::is_same<nostd::variant_alternative_t<1, nostd::variant<int, double>>, double>::value));
  EXPECT_TRUE((std::is_same<nostd::variant_alternative_t<1, const nostd::variant<int, double>>,
                            const double>::value));
}

TEST(VariantTest, Get)
{
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

TEST(VariantTest, Comparison)
{
  nostd::variant<int, float> v, w;
  EXPECT_TRUE(v == w);
  EXPECT_FALSE(v != w);
  v = 3.0f;
  EXPECT_TRUE(v != w);
  EXPECT_FALSE(v == w);
  v = 2;
  w = 3;
  EXPECT_TRUE(v != w);
  EXPECT_FALSE(v == w);
  EXPECT_TRUE(v < w);
  EXPECT_FALSE(v > w);
}

TEST(VariantTest, Visit)
{
  nostd::variant<int, float> v;
  struct
  {
    int operator()(int) { return 0; }
    int operator()(float) { return 1; }
  } a;
  EXPECT_EQ(nostd::visit(a, v), 0);
  v = 2.0f;
  EXPECT_EQ(nostd::visit(a, v), 1);
}

TEST(VariantTest, Destructor)
{
  nostd::variant<int, DestroyCounter> v;
  int destroy_count = 0;
  v                 = DestroyCounter{&destroy_count};
  destroy_count     = 0;
  v                 = 1;
  EXPECT_EQ(destroy_count, 1);
  {
    nostd::variant<int, DestroyCounter> w;
    w             = DestroyCounter{&destroy_count};
    destroy_count = 0;
  }
  EXPECT_EQ(destroy_count, 1);
}

TEST(VariantTest, Conversion)
{
  nostd::variant<std::string> x("abc");
  x = "def";
  EXPECT_EQ(nostd::get<std::string>(x), "def");

  nostd::variant<std::string, void const *> y("abc");
  EXPECT_TRUE(nostd::holds_alternative<void const *>(y));
  y = std::string{"xyz"};
  EXPECT_TRUE(nostd::holds_alternative<std::string>(y));
}

TEST(VariantTest, Construction)
{
  nostd::variant<bool, std::string> v{"abc"};
  EXPECT_EQ(v.index(), 1);
}
