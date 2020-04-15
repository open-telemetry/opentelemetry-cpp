#include "src/event/libevent/timer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
Timer::Timer(EventBase &event_base, Callback callback) noexcept
    : event_{event_base, -1, OnTimeout, 0, this}, callback_{std::move(callback)}
{}

void Timer::EnableTimer(std::chrono::microseconds timeout) noexcept {
  event_.Add(timeout);
}

void Timer::DisableTimer() noexcept {
  event_.Delete();
}

void Timer::OnTimeout(FileDescriptor /*file_descriptor*/, short /*what*/, void *context) noexcept
{
  static_cast<Timer *>(context)->callback_();
}
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
