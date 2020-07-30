#include "src/event/libevent/event.h"

#include <iostream>

#include <event2/event.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
static void ToTimeval(std::chrono::microseconds timeout, timeval &tv) noexcept
{
  auto num_microseconds               = timeout.count();
  const size_t microseconds_in_second = 1000000;
  tv.tv_sec  = static_cast<decltype(tv.tv_sec)>(num_microseconds / microseconds_in_second);
  tv.tv_usec = static_cast<decltype(tv.tv_usec)>(num_microseconds % microseconds_in_second);
}

Event::Event(EventBase &event_base,
             FileDescriptor file_descriptor,
             Callback callback,
             short what,
             void *context) noexcept
{
  event_ = ::event_new(event_base.libevent_handle(), file_descriptor, what, callback, context);
  if (event_ == nullptr)
  {
    std::cerr << "event_new failed\n";
    std::terminate();
  }
}

Event::~Event() noexcept
{
  ::event_free(event_);
}

void Event::Add(std::chrono::microseconds timeout) noexcept
{
  int rcode;
  if (timeout.count() == 0)
  {
    rcode = ::event_add(event_, nullptr);
  }
  else
  {
    timeval tv;
    ToTimeval(timeout, tv);
    rcode = ::event_add(event_, &tv);
  }
  if (rcode == -1)
  {
    std::cerr << "event_add failed\n";
    std::terminate();
  }
}

void Event::Delete() noexcept
{
  auto rcode = ::event_del(event_);
  if (rcode == -1)
  {
    std::cerr << "event_del failed\n";
    std::terminate();
  }
}
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
