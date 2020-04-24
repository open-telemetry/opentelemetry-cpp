#pragma once

#include "opentelemetry/sdk/event/timer.h"
#include "opentelemetry/version.h"

#include "src/event/async_timer/dispatcher.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer {
class Timer final : event::Timer {
 public:
  explicit Timer(Dispatcher::EventIterator iterator) noexcept;

  void EnableTimer(std::chrono::microseconds timeout) noexcept override;

  void DisableTimer() noexcept override;
 private:
  Dispatcher::EventIterator iterator_;
};
} // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
