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

   virtual void DisableTimer() noexcept = 0;
};
} // namespace event
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
