#include "opentelemetry/nostd/string_view.h"

#include <gtest/gtest.h>

using opentelemetry::nostd::string_view;

TEST(StringViewTest, DefaultConstruction)
{
  string_view ref;
  EXPECT_EQ(ref.data(), nullptr);
  EXPECT_EQ(ref.length(), 0);
}

TEST(StringViewTest, CStringInitialization)
{
  const char *val = "hello world";

  string_view ref(val);

  EXPECT_EQ(ref.data(), val);
  EXPECT_EQ(ref.length(), std::strlen(val));
}

TEST(StringViewTest, StdStringInitialization)
{
  const std::string val = "hello world";

  string_view ref(val);

  EXPECT_EQ(ref.data(), val.data());
  EXPECT_EQ(ref.length(), val.size());
}

TEST(StringViewTest, Copy)
{
  const std::string val = "hello world";

  string_view ref(val);
  string_view cpy(ref);

  EXPECT_EQ(cpy.data(), val);
  EXPECT_EQ(cpy.length(), val.length());
  EXPECT_EQ(cpy, val);
}

TEST(StringViewTest, Accessor)
{
  string_view s = "abc123";
  EXPECT_EQ(s.data(), &s[0]);
  EXPECT_EQ(s.data() + 1, &s[1]);
}

TEST(StringViewTest, ExplicitStdStringConversion)
{
  std::string s = static_cast<std::string>(string_view{"abc"});
  EXPECT_EQ(s, "abc");
}

TEST(StringViewTest, SubstrPortion)
{
  string_view s = "abc123";
  EXPECT_EQ("123", s.substr(3));
  EXPECT_EQ("12", s.substr(3, 2));
}

TEST(StringViewTest, SubstrOutOfRange)
{
  string_view s = "abc123";
  EXPECT_THROW(s.substr(10), std::out_of_range);
}
