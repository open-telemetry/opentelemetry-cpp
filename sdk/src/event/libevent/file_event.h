#pragma once

#include <functional>

#include "src/event/libevent/event.h"

#include "opentelemetry/sdk/event/file_event.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
class FileEvent final : public event::FileEvent
{
public:
  using Callback = std::function<void(uint32_t)>;

  FileEvent(EventBase &event_base,
            FileDescriptor file_descriptor,
            uint32_t events,
            Callback callback) noexcept;

private:
  Event event_;
  Callback callback_;

  static void OnFileEvent(FileDescriptor file_descriptor, short what, void *context) noexcept;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
