// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
// version.h should be included before nostd/variant.h.
#include "opentelemetry/version.h"
#include "opentelemetry/nostd/variant.h"
// clang-format on

#if defined(HAVE_ABSEIL) && !defined(HAVE_ABSEIL_VARIANT)

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
