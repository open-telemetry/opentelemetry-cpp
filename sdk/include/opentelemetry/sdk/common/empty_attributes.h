#include "opentelemetry/trace/key_value_iterable_view.h"

#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
/**
 * Maintain a static empty array of pairs that represents empty (default) attributes.
 * This helps to avoid constructing a new empty container every time a call is made
 * with default attributes.
 */
static const opentelemetry::trace::KeyValueIterableView<std::array<std::pair<std::string, int>, 0>>
    &GetEmptyAttributes() noexcept
{
  static const std::array<std::pair<std::string, int>, 0> array{};
  static const opentelemetry::trace::KeyValueIterableView<
      std::array<std::pair<std::string, int>, 0>>
      kEmptyAttributes(array);

  return kEmptyAttributes;
}
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
