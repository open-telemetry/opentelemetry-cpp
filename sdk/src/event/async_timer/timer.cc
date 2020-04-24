#include "src/event/async_timer/timer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
Timer::Timer(Dispatcher::EventIterator iterator) noexcept : iterator_{iterator} {}

void Timer::EnableTimer(std::chrono::microseconds timeout) noexcept
{
  (void)timeout;
}

void Timer::DisableTimer() noexcept {}
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
