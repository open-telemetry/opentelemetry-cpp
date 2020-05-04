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
class Timer;

class Dispatcher final : public event::Dispatcher
{
public:
  // event::Dispatcher
  std::unique_ptr<event::Timer> CreateTimer(TimerCallback callback) noexcept override;

  void Exit() noexcept override;

  void Run() noexcept override;

private:
  struct Event {
    TimerCallback callback;
    Timer* timer;
  };
  using EventIterator =
      std::map<std::chrono::steady_clock::time_point, Event>::const_iterator;

  friend class Timer;
  bool running_{true};
  std::multimap<std::chrono::steady_clock::time_point, Event> events_;
};
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
