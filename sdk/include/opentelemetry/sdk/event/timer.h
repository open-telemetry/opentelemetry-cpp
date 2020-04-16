#pragma once

#include <chrono>
#include <system_error>
#include <functional>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk {
namespace event {
using TimerCallback = std::function<void()>;

/**
 * Manages a timer event.
 *
 * Note: Must be freed before the dipatcher is destructed
 */
class Timer {
 public:
   virtual ~Timer() = default;

   /**
    * Enable a pending timeout. If a timeout is already pending, it will be reset to the new
    * timeout.
    */
   virtual void EnableTimer(std::chrono::microseconds timeout) noexcept = 0;

   template <class Rep, class Period>
   void EnableTimer(std::chrono::duration<Rep, Period> timeout) noexcept
   {
     this->EnableTimer(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
   }

   /**
    * Disable a pending timeout.
    *
    * If no timeout is active, it does nothing.
    */
   virtual void DisableTimer() noexcept = 0;
};
} // namespace event
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
