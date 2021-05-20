// Copyright 2020, OpenTelemetry Authors
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

#pragma once

#ifdef HAVE_CPP_STDLIB
#  include "opentelemetry/std/variant.h"
#else

#ifndef HAVE_ABSEIL
// We use a LOCAL snapshot of Abseil that is known to compile with Visual Studio 2015.
// Header-only. Without compiling the actual Abseil binary. As Abseil moves on to new
// toolchains, it may drop support for Visual Studio 2015 in future versions.
#  include <exception>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{

class bad_variant_access : public std::exception
{
public:
  virtual const char *what() const noexcept override { return "bad_variant_access"; }
};

[[noreturn]] inline void throw_bad_variant_access()
{
  throw bad_variant_access{};
}
}
OPENTELEMETRY_END_NAMESPACE

#  if defined(__EXCEPTIONS) || defined(__cpp_exceptions)
#    define THROW_BAD_VARIANT_ACCESS opentelemetry::nostd::throw_bad_variant_access()
#  else
#    define THROW_BAD_VARIANT_ACCESS std::terminate()
#  endif
#endif

#  ifdef _MSC_VER
// Abseil variant implementation contains some benign non-impacting warnings
// that should be suppressed if compiling with Visual Studio 2017 and above.
#    pragma warning(push)
#    pragma warning(disable : 4245)  // conversion from int to const unsigned _int64
#    pragma warning(disable : 4127)  // conditional expression is constant
#  endif
#  include "absl/types/variant.h"
#  ifdef _MSC_VER
#    pragma warning(pop)
#  endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
using absl::get;
using absl::holds_alternative;
using absl::monostate;
using absl::variant;
using absl::variant_size;
using absl::visit;

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

#endif
