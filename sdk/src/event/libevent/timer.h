#pragma once

#include <functional>

#include "src/event/libevent/event.h"

#include "opentelemetry/sdk/event/timer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
class Timer final : public event::Timer {
 public:
   using Callback = std::function<void()>;

   Timer(EventBase& event_base, Callback callback) noexcept;

   void EnableTimer(std::chrono::microseconds timeout) noexcept override;

   void DisableTimer() noexcept override;

 private:
   Event event_;
   Callback callback_;

   static void OnTimeout(FileDescriptor /*file_descriptor*/,
                         short /*what*/,
                         void *context) noexcept;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
