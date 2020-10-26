#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

#if defined(HAVE_ABSEIL)
namespace absl
{
namespace variant_internal
{
void __cdecl ThrowBadVariantAccess(){/* TODO: std::terminate or re-throw? */};
}
}  // namespace absl
#endif
