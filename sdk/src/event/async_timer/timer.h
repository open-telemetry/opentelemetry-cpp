#pragma once

#include "opentelemetry/sdk/event/timer.h"
#include "opentelemetry/version.h"

#include "src/event/async_timer/dispatcher.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
class Timer final : public event::Timer
{
public:
  Timer(TimerCallback callback, Dispatcher &dispatcher) noexcept;

  ~Timer();

  void EnableTimer(std::chrono::microseconds timeout) noexcept override;

  void DisableTimer() noexcept override;

private:
  friend class Dispatcher;
  TimerCallback callback_;
  Dispatcher &dispatcher_;
  Dispatcher::EventIterator event_;
};
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
