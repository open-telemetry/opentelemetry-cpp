#include "src/common/platform/fork.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace api
{
namespace common
{
namespace platform
{
int AtFork(void (*prepare)(), void (*parent)(), void (*child)()) noexcept
{
  (void)prepare;
  (void)parent;
  (void)child;
  return 0;
}
}  // namespace platform
}  // namespace common
}  // namespace api
OPENTELEMETRY_END_NAMESPACE
