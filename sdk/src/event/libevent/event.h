#pragma once

#include <chrono>

#include "src/event/libevent/event_base.h"

#include "opentelemetry/sdk/common/file_descriptor.h"
#include "opentelemetry/version.h"

struct event;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
class Event
{
public:
  using Callback = void (*)(FileDescriptor, short what, void *context);

  Event(EventBase &event_base,
        FileDescriptor file_descriptor,
        Callback callback,
        short what,
        void *context) noexcept;

  ~Event() noexcept;

  virtual void Add(std::chrono::microseconds timeout = std::chrono::microseconds{0}) noexcept;

  virtual void Delete() noexcept;

private:
  ::event *event_;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
