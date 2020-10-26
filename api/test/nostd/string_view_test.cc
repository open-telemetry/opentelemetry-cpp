#include "opentelemetry/nostd/string_view.h"

#include <gtest/gtest.h>

#include <benchmark/benchmark.h>
#include <cstdint>
#include <cstring>
#include <map>

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
#if __EXCEPTIONS || defined(HAVE_STDLIB_CPP)
  EXPECT_THROW(s.substr(10), std::out_of_range);
#else
  EXPECT_DEATH({ s.substr(10); }, "");
#endif
}

TEST(StringViewTest, Compare)
{
  string_view s1 = "aaa";
  string_view s2 = "bbb";
  string_view s3 = "aaa";

  // Equals
  EXPECT_EQ(s1, s3);
  EXPECT_EQ(s1, s1);

  // Less then
  EXPECT_LT(s1, s2);

  // Greater then
  EXPECT_GT(s2, s1);
}

TEST(StringViewTest, MapKeyOrdering)
{
  std::map<string_view, size_t> m = {{"bbb", 2}, {"aaa", 1}, {"ccc", 3}};
  size_t i                        = 1;
  for (const auto &kv : m)
  {
    EXPECT_EQ(kv.second, i);
    i++;
  }
}

static void StringViewSubStr(benchmark::State &state)
{
  std::string s =
      "Hello OpenTelemetry nostd::string_view implementation! Feel free to evaluate my "
      "performance.";
  for (auto _ : state)
  {
    string_view sv = s;
    auto oneSv     = sv.substr(0, 5);
    auto twoSv     = sv.substr(6, 5);
    auto threeSv   = sv.substr(12, 5);
    auto fourSv    = sv.substr(18, 11);
    auto fiveSv    = sv.substr(30, 5);
    benchmark::DoNotOptimize(oneSv);
    benchmark::DoNotOptimize(twoSv);
    benchmark::DoNotOptimize(threeSv);
    benchmark::DoNotOptimize(fourSv);
    benchmark::DoNotOptimize(fiveSv);
  }
}
BENCHMARK(StringViewSubStr);

static void StringViewMaps(benchmark::State &state)
{
  std::map<string_view, string_view> m;
  std::string txt;
  size_t i = 0;
  for (auto _ : state)
  {
    i %= 200;  // up to 200 key-value pairs in this collection
    m[string_view(std::to_string(i))] = string_view(std::to_string(i));
    i += 1;
  };
}
BENCHMARK(StringViewMaps);

static void StringViewFromCString(benchmark::State &state)
{
  std::string s =
      "Hello OpenTelemetry nostd::string_view implementation! Feel free to evaluate my "
      "performance.";
  for (auto _ : state)
  {
    string_view sv(s.c_str());
    // scan thru string_view
    for (const auto &c : sv)
      ;
  };
}
BENCHMARK(StringViewFromCString);

static void StringViewToString(benchmark::State &state)
{
  string_view s =
      "Hello OpenTelemetry nostd::string_view implementation! Feel free to evaluate my "
      "performance.";
  std::string txt;
  for (auto _ : state)
  {
    txt = std::string(s.data(), s.length());
  };
}
BENCHMARK(StringViewToString);

static void StringViewExplode(benchmark::State &state)
{
  for (auto _ : state)
  {
    string_view sv =
        "Hello OpenTelemetry nostd::string_view implementation! Feel free to evaluate my "
        "performance.";
    std::string s;
    for (size_t i = 0; i < 5; i++)
    {
      s += std::string(sv.data(), sv.length());
      sv              = s;
      string_view sv2 = s;
      if (sv == sv2)
        ;  // FIXME: Warning C4390 ';' : empty controlled statement found; is this the intent ?
    }
  };
}
BENCHMARK(StringViewExplode);

static void StringViewVector(benchmark::State &state)
{
  std::vector<string_view> v;
  for (auto _ : state)
  {
    string_view sv =
        "Hello OpenTelemetry nostd::string_view implementation! Feel free to evaluate my "
        "performance.";
    v.push_back(sv);
    if (v.size() > 10000)
      v.clear();
  }
}
BENCHMARK(StringViewVector);

TEST(StringView, PerfTests)
{
  // Run all benchmarks
  int argc           = 0;
  const char *argv[] = {""};
  ::benchmark::Initialize(&argc, (char **)(argv));
  ::benchmark::RunSpecifiedBenchmarks();
}
