#include "opentelemetry/nostd/unique_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::unique_ptr;

class A {
 public:
  explicit A(bool& destructed) noexcept : destructed_{destructed} {
    destructed_ = false;
  }

  ~A() { destructed_ = true; }

private:
  bool &destructed_;
};

class B {
 public:
   int f() const { return 123; }
};

TEST(UniquePtrTest, DefaultConstruction)
{
  unique_ptr<int> ptr1;
  EXPECT_EQ(ptr1.get(), nullptr);

  unique_ptr<int> ptr2{nullptr};
  EXPECT_EQ(ptr2.get(), nullptr);
}

TEST(UniquePtrTest, ExplicitConstruction) {
  auto value = new int{123};
  unique_ptr<int> ptr{value};
  EXPECT_EQ(ptr.get(), value);
}

TEST(UniquePtrTest, MoveConstruction) {
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, MoveConstructionFromDifferentType) {
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  unique_ptr<const int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, MoveConstructionFromStdUniquePtr) {
  auto value = new int{123};
  std::unique_ptr<int> ptr1{value};
  unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);
}

TEST(UniquePtrTest, Destruction) {
  bool was_destructed;
  unique_ptr<A>{new A{was_destructed}};
  EXPECT_TRUE(was_destructed);
}

TEST(UniquePtrTest, StdUniquePtrConversionOperator) {
  auto value = new int{123};
  unique_ptr<int> ptr1{value};
  std::unique_ptr<int> ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);

  value = new int{456};
  ptr1 = unique_ptr<int>{value};
  ptr2 = std::move(ptr1);
  EXPECT_EQ(ptr1.get(), nullptr);
  EXPECT_EQ(ptr2.get(), value);

  EXPECT_TRUE((std::is_assignable<std::unique_ptr<int>, unique_ptr<int>&&>::value));
  EXPECT_TRUE(!(std::is_assignable<std::unique_ptr<int>, unique_ptr<int>&>::value));
}

TEST(UniquePtrTest, BoolConversionOpertor) {
  auto value = new int{123};
  unique_ptr<int> ptr1{value};

  EXPECT_TRUE(ptr1);
  EXPECT_FALSE(unique_ptr<int>{});
}

TEST(UniquePtrTest, PointerOperators) {
  auto value = new int{123};
  unique_ptr<int> ptr1{value};

  EXPECT_EQ(&*ptr1, value);
  EXPECT_EQ(unique_ptr<B>{}->f(), 123);
}

TEST(UniquePtrTest, Swap) {
  auto value1 = new int{123};
  unique_ptr<int> ptr1{value1};

  auto value2 = new int{456};
  unique_ptr<int> ptr2{value2};
  ptr1.swap(ptr2);

  EXPECT_EQ(ptr1.get(), value2);
  EXPECT_EQ(ptr2.get(), value1);
}
