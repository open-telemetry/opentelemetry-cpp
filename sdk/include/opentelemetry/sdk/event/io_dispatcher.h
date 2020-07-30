#pragma once

#include "opentelemetry/sdk/event/dispatcher.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
class IoDispatcher : public Dispatcher
{
public:
  /**
   * Create an event that triggers when a file descriptor becomes readable or writable.
   * @param file_descriptor the file descriptor to monitor
   * @param callback the callback to call when event triggers
   * @param events the events to monitor
   * @return a handle for the event
   */
  virtual std::unique_ptr<FileEvent> CreateFileEvent(FileDescriptor file_descriptor,
                                                     FileReadyCallback callback,
                                                     uint32_t events) noexcept = 0;
};
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
