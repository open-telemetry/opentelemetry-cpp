#pragma once

#include <map>

#include "opentelemetry/sdk/event/dispatcher.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
class Dispatcher final : public event::Dispatcher
{
public:
  using EventIterator =
      std::map<std::chrono::steady_clock::time_point, TimerCallback>::const_iterator;

  // event::Dispatcher
  std::unique_ptr<event::Timer> CreateTimer(TimerCallback callback) noexcept override;

  void Exit() noexcept override;

  void Run() noexcept override;

private:
  friend class Timer;
  bool running_{true};
  std::multimap<std::chrono::steady_clock::time_point, TimerCallback> events_;
};
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
