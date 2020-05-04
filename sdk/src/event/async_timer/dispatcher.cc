#include "src/event/async_timer/dispatcher.h"

#include <thread>

#include "src/event/async_timer/timer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
std::unique_ptr<event::Timer> Dispatcher::CreateTimer(TimerCallback callback) noexcept
{
  return std::unique_ptr<event::Timer>{new (std::nothrow) Timer{callback, *this}};
}

void Dispatcher::Exit() noexcept
{
  running_ = false;
}

void Dispatcher::Run() noexcept
{
  while (running_ && !events_.empty())
  {
    auto next_event = events_.begin();
    std::this_thread::sleep_until(next_event->first);
    if (next_event->second.callback)
    {
      next_event->second.callback();
    }
    next_event->second.timer->event_ = events_.end();
    events_.erase(next_event);
  }
}
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
