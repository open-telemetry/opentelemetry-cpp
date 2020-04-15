#include "src/event/libevent/dispatcher.h"

#include "event2/event.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event {
namespace libevent {
std::unique_ptr<FileEvent> Dispatcher::CreateFileEvent(FileDescriptor file_descriptor,
                                                               FileReadyCallback callback,
                                                               uint32_t events) noexcept
{
  (void)file_descriptor;
  (void)callback;
  (void)events;
  return nullptr;
}

std::unique_ptr<Timer> Dispatcher::CreateTimer(TimerCallback callback) noexcept {
  (void)callback;
  return nullptr;
}

void Dispatcher::Exit() noexcept {
  event_base_.LoopBreak();
}


void Dispatcher::Run() noexcept {
  event_base_.Dispatch();
}
} // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
