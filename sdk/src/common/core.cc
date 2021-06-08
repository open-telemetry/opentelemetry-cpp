// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
// version.h should be included before nostd/variant.h.
#include "opentelemetry/version.h"
#include "opentelemetry/nostd/variant.h"
// clang-format on

#if !defined(HAVE_CPP_STDLIB) && !defined(OTABSL_USES_STD_VARIANT)
/* The option of building and linking with Abseil library implies that Abseil
 * may already provide the ThrowBadVariantAccess implementation if its own.
 * Reconsider the implementation below: we are potentially introducing
 * a function that is already implemented in the Abseil. Most likely the code
 * below needs to be removed entirely.
 */
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
#  if defined(HAVE_ABSEIL)
ABSL_NAMESPACE_BEGIN
#  else
OTABSL_NAMESPACE_BEGIN
#  endif
namespace variant_internal
{
void __cdecl ThrowBadVariantAccess(){/* TODO: std::terminate or re-throw? */};
}
#  if defined(HAVE_ABSEIL)
ABSL_NAMESPACE_END
#  else
OTABSL_NAMESPACE_END
#  endif
}  // namespace absl
#endif
