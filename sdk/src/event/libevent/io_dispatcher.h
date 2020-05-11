#pragma once

#include "opentelemetry/sdk/event/io_dispatcher.h"
#include "opentelemetry/version.h"

#include "src/event/libevent/event_base.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
/**
 * Manages asynchronous events on a file descriptor.
 *
 * Built on top of libevent https://libevent.org/
 */
class IoDispatcher final : public event::IoDispatcher
{
public:
  // Dispatcher
  std::unique_ptr<event::FileEvent> CreateFileEvent(FileDescriptor file_descriptor,
                                                    FileReadyCallback callback,
                                                    uint32_t events) noexcept override;

  std::unique_ptr<event::Timer> CreateTimer(TimerCallback callback) noexcept override;

  void Exit() noexcept override;

  void Run() noexcept override;

private:
  EventBase event_base_;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
