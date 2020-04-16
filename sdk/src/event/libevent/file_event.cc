#include "src/event/libevent/file_event.h"

#include <cassert>

#include "event2/event.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace libevent
{
static short ToLibeventWhat(uint32_t events) noexcept
{
  short result = 0;
  if (events & FileReadyType::kRead)
  {
    result |= EV_READ;
  }
  if (events & FileReadyType::kWrite)
  {
    result |= EV_WRITE;
  }
  if (events & FileReadyType::kClosed)
  {
    result |= EV_CLOSED;
  }
  assert(events != 0);
  return result;
}

static uint32_t FromLibeventWhat(short what) noexcept
{
  uint32_t result = 0;
  if (what & EV_READ)
  {
    result |= FileReadyType::kRead;
  }
  if (what & EV_WRITE)
  {
    result |= FileReadyType::kWrite;
  }
  if (what & EV_CLOSED)
  {
    result |= FileReadyType::kClosed;
  }
  return result;
}

FileEvent::FileEvent(EventBase &event_base,
                     FileDescriptor file_descriptor,
                     uint32_t events,
                     Callback callback) noexcept
    : event_{event_base, file_descriptor, OnFileEvent, ToLibeventWhat(events), this},
      callback_{std::move(callback)}
{
  assert(callback_);
  event_.Add();
}

void FileEvent::OnFileEvent(FileDescriptor /*file_descriptor*/, short what, void *context) noexcept
{
  static_cast<FileEvent *>(context)->callback_(FromLibeventWhat(what));
}
}  // namespace libevent
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
