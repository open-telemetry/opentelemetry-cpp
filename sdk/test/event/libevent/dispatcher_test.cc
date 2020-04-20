#include "src/event/libevent/dispatcher.h"

#include <chrono>

#include <gtest/gtest.h>
using opentelemetry::sdk::event::Timer;
using opentelemetry::sdk::event::libevent::Dispatcher;

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

TEST(DispatcherTest, Exit)
{
  Dispatcher dispatcher;
  std::unique_ptr<Timer> timer;
  auto f = [&] {
    timer->EnableTimer(std::chrono::milliseconds{1});
    dispatcher.Exit();
  };
  timer = dispatcher.CreateTimer(f);
  dispatcher.Run();
}
