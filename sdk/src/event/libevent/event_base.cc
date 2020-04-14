#include "src/event/libevent/event_base.h"

#include <cassert>
#include <iostream>

#include <event2/event.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
EventBase::EventBase() noexcept
{
  event_base_ = ::event_base_new();
  if (event_base_ == nullptr)
  {
    std::cerr << "event_base_new failed\n";
    std::terminate();
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
EventBase::~EventBase()
{
  ::event_base_free(event_base_);
}

//------------------------------------------------------------------------------
// Dispatch
//------------------------------------------------------------------------------
void EventBase::Dispatch() const noexcept
{
  auto rcode = ::event_base_dispatch(event_base_);
  if (rcode == -1)
  {
    std::cerr << "event_base_dispatch failed\n";
    std::terminate();
  }
}

//------------------------------------------------------------------------------
// LoopBreak
//------------------------------------------------------------------------------
void EventBase::LoopBreak() const noexcept
{
  auto rcode = ::event_base_loopbreak(event_base_);
  if (rcode == -1)
  {
    std::cerr << "event_base_loopbreak failed\n";
    std::terminate();
  }
}
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
