#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

#if defined(HAVE_ABSEIL)

#  if defined(__GNUC__) || defined(__GNUG__)
#    ifndef __cdecl
// see https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
// Intel x86 architecture specific calling conventions
#      ifdef _M_IX86
#        define __cdecl __attribute__((__cdecl__))
#      else
#        define __cdecl
#      endif
#    endif
#  endif

namespace absl
{
namespace variant_internal
{
void __cdecl ThrowBadVariantAccess(){/* TODO: std::terminate or re-throw? */};
}
}  // namespace absl
#endif
