// clang-format off
// version.h should be included before nostd/variant.h.
#include "opentelemetry/version.h"
#include "opentelemetry/nostd/variant.h"
// clang-format on

#if defined(HAVE_ABSEIL) && !defined(HAVE_ABSEIL_VARIANT)
namespace absl
{
namespace variant_internal
{
void __cdecl ThrowBadVariantAccess(){/* TODO: std::terminate or re-throw? */};
}
}  // namespace absl
#endif
