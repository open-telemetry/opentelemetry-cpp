// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
