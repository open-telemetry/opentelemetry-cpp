#include "opentelemetry/trace/key_value_iterable_view.h"

#include <array>
#include <map>
#include <string>
#include <utility>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

static const opentelemetry::trace::NullKeyValueIterable GetEmptyAttributes() noexcept
{
  return opentelemetry::trace::NullKeyValueIterable();
}
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
