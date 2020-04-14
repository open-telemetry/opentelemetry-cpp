#pragma once

#include "opentelemetry/sdk/event/dispatcher.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
class Dispatcher final : public event::Dispatcher
{
public:
  // Dispatcher
  std::unique_ptr<FileEvent> CreateFileEvent(FileDescriptor file_descriptor,
                                             FileReadyCallback callback,
                                             uint32_t events) noexcept override;

  std::unique_ptr<Timer> CreateTimer(TimerCallback callback) noexcept override;

  void Exit() noexcept override;

  void Run() noexcept override;
};
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
