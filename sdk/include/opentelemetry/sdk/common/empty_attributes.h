#include "opentelemetry/trace/key_value_iterable_view.h"

#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
/**
 * Maintain a static empty map that represents empty (default) attributes.
 * Helps to avoid constructing a new empty map everytime a call is made with default attributes.
 */
static const opentelemetry::trace::KeyValueIterableView<std::map<std::string, int>>
    &GetEmptyAttributes() noexcept
{
  static const std::map<std::string, int> map;
  static const opentelemetry::trace::KeyValueIterableView<std::map<std::string, int>>
      kEmptyAttributes(map);
  return kEmptyAttributes;
}
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
