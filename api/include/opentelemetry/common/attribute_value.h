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

#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
/// OpenTelemetry signals can be enriched by adding attributes. The
/// \c AttributeValue type is defined as a variant of all attribute value
/// types the OpenTelemetry C++ API supports.
///
/// The following attribute value types are supported by the OpenTelemetry
/// specification:
///  - Primitive types: string, boolean, double precision floating point
///    (IEEE 754-1985) or signed 64 bit integer.
///  - Homogenous arrays of primitive type values.
///
/// \warning
/// \parblock The OpenTelemetry C++ API currently supports several attribute
/// value types that are not covered by the OpenTelemetry specification:
///  - \c uint64_t
///  - \c nostd::span<const uint64_t>
///  - \c nostd::span<uint8_t>
///
/// Those types are reserved for future use and currently should not be
/// used. There are no guarantees around how those values are handled by
/// exporters.
/// \endparblock
using AttributeValue =
    nostd::variant<bool,
                   int32_t,
                   int64_t,
                   uint32_t,
                   double,
                   nostd::string_view,
                   nostd::span<const bool>,
                   nostd::span<const int32_t>,
                   nostd::span<const int64_t>,
                   nostd::span<const uint32_t>,
                   nostd::span<const double>,
                   nostd::span<const nostd::string_view>,
                   // Not currently supported by the specification, but reserved for future use.
                   // Added to provide support for all primitive C++ types.
                   uint64_t,
                   // Not currently supported by the specification, but reserved for future use.
                   // Added to provide support for all primitive C++ types.
                   nostd::span<const uint64_t>,
                   // Not currently supported by the specification, but reserved for future use.
                   // See https://github.com/open-telemetry/opentelemetry-specification/issues/780
                   nostd::span<const uint8_t>>;

enum AttributeType
{
  kTypeBool,
  kTypeInt,
  kTypeInt64,
  kTypeUInt,
  kTypeDouble,
  kTypeString,
  kTypeSpanBool,
  kTypeSpanInt,
  kTypeSpanInt64,
  kTypeSpanUInt,
  kTypeSpanDouble,
  kTypeSpanString,
  kTypeUInt64,
  kTypeSpanUInt64,
  kTypeSpanByte
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
