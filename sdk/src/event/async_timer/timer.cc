#include "src/event/async_timer/timer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
Timer::Timer(TimerCallback callback, Dispatcher &dispatcher) noexcept
    : callback_{callback}, dispatcher_{dispatcher}, event_{dispatcher.events_.end()}
{}

Timer::~Timer()
{
  this->DisableTimer();
}

void Timer::EnableTimer(std::chrono::microseconds timeout) noexcept
{
  this->DisableTimer();
  auto time_point = std::chrono::steady_clock::now() + timeout;
  // Note: terminates on std::bad_alloc
  event_ = dispatcher_.events_.emplace(time_point, callback_);
}

void Timer::DisableTimer() noexcept
{
  if (event_ != dispatcher_.events_.end())
  {
    dispatcher_.events_.erase(event_);
  }
}
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
