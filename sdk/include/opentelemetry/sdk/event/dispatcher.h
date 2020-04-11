#pragma once

#include <memory>

#include "opentelemetry/sdk/common/file_descriptor.h"
#include "opentelemetry/sdk/event/file_event.h"
#include "opentelemetry/sdk/event/timer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk {
namespace event {
class Dispatcher {
 public:
  virtual ~Dispatcher() = default;

  virtual std::unique_ptr<FileEvent> CreateFileEvent(FileDescriptor file_descriptor,
                                                     FileReadyCallback callback,
                                                     uint32_t events) noexcept = 0;

  virtual std::unique_ptr<Timer> CreateTimer(TimerCallback callback) noexcept = 0;

  virtual void Exit() noexcept = 0;

  virtual void Run() noexcept = 0;
};
} // namespace event
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
