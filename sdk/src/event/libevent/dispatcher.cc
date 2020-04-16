#include "src/event/libevent/dispatcher.h"
#include "src/event/libevent/timer.h"
#include "src/event/libevent/file_event.h"

#include "event2/event.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event {
namespace libevent {
std::unique_ptr<event::FileEvent> Dispatcher::CreateFileEvent(FileDescriptor file_descriptor,
                                                              FileReadyCallback callback,
                                                              uint32_t events) noexcept
{
  return std::unique_ptr<event::FileEvent>{
      new (std::nothrow) FileEvent{event_base_, file_descriptor, events, callback}};
}

std::unique_ptr<event::Timer> Dispatcher::CreateTimer(TimerCallback callback) noexcept
{
  return std::unique_ptr<event::Timer>{new (std::nothrow) Timer{event_base_, callback}};
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
