#pragma once

#include <memory>

#include "opentelemetry/sdk/common/file_descriptor.h"
#include "opentelemetry/sdk/event/file_event.h"
#include "opentelemetry/sdk/event/timer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
/**
 * Interface for managing asynchronous events.
 */
class Dispatcher
{
public:
  virtual ~Dispatcher() = default;

  /**
   * Create a timer event.
   * @callback the callback to call when the event triggers
   * @return a handle for the timer
   */
  virtual std::unique_ptr<Timer> CreateTimer(TimerCallback callback) noexcept = 0;

  /**
   * Stop the event loop.
   */
  virtual void Exit() noexcept = 0;

  /**
   * Run the event loop until all events have been processed or Exit is called.
   */
  virtual void Run() noexcept = 0;
};
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
