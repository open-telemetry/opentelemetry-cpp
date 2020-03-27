#include "src/common/platform/fork.h"

#include <pthread.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace platform
{
int AtFork(void (*prepare)(), void (*parent)(), void (*child)()) noexcept
{
  return ::pthread_atfork(prepare, parent, child);
}
}  // namespace platform
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
