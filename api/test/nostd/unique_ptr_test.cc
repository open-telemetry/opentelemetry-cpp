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
