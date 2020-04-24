#include "src/event/async_timer/dispatcher.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
namespace async_timer
{
std::unique_ptr<event::Timer> Dispatcher::CreateTimer(TimerCallback callback) noexcept
{
  (void)callback;
  return nullptr;
}

void Dispatcher::Exit() noexcept {}

void Dispatcher::Run() noexcept {}
}  // namespace async_timer
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

