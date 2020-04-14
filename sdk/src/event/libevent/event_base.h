#pragma once

#include <chrono>

struct event_base;

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
/**
 * Wrapper for libevent's event_base struct.
 */
class EventBase
{
public:
  EventBase() noexcept;

  EventBase(EventBase &&other) = delete;
  EventBase(const EventBase &) = delete;

  ~EventBase();

  EventBase &operator=(EventBase &&other) noexcept = delete;
  EventBase &operator=(const EventBase &) = delete;

  /**
   * @return the underlying event_base.
   */
  event_base *libevent_handle() const noexcept { return event_base_; }

  /**
   * Run the dispatch event loop.
   */
  void Dispatch() const noexcept;

  /**
   * Break out of the dispatch event loop.
   */
  void LoopBreak() const noexcept;

private:
  event_base *event_base_;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
