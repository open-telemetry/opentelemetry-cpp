#include "src/event/async_timer/dispatcher.h"

#include <chrono>

#include <gtest/gtest.h>
using opentelemetry::sdk::event::Timer;
using opentelemetry::sdk::event::async_timer::Dispatcher;

TEST(DispatcherTest, Timer)
{
  Dispatcher dispatcher;
  std::chrono::steady_clock::time_point t1, t2;
  auto timer = dispatcher.CreateTimer([&] { t2 = std::chrono::steady_clock::now(); });
  timer->EnableTimer(std::chrono::milliseconds{100});
  t1 = std::chrono::steady_clock::now();
  dispatcher.Run();
  auto duration = t2 - t1;
  EXPECT_TRUE(duration > std::chrono::milliseconds{50});
  EXPECT_TRUE(duration < std::chrono::milliseconds{200});
}

TEST(DispatcherTest, TimerOrder)
{
  Dispatcher dispatcher;
  std::vector<int> order;

  auto t1 = dispatcher.CreateTimer([&] { order.push_back(1); });
  t1->EnableTimer(std::chrono::milliseconds{5});

  auto t2 = dispatcher.CreateTimer([&] { order.push_back(2); });
  t2->EnableTimer(std::chrono::milliseconds{2});

  auto t3 = dispatcher.CreateTimer([&] { order.push_back(3); });
  t3->EnableTimer(std::chrono::milliseconds{7});

  dispatcher.Run();

  EXPECT_EQ(order, (std::vector<int>{2, 1, 3}));
}
