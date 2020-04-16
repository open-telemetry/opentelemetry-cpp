#pragma once

#include <chrono>
#include <system_error>
#include <functional>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk {
namespace event {
using TimerCallback = std::function<void()>;

class Timer {
 public:
   virtual ~Timer() = default;

   virtual void EnableTimer(std::chrono::microseconds timeout) noexcept = 0;

   template <class Rep, class Period>
   void EnableTimer(std::chrono::duration<Rep, Period> timeout) noexcept
   {
     this->EnableTimer(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
   }

   virtual void DisableTimer() noexcept = 0;
};
} // namespace event
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
