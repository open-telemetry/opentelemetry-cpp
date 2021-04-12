#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
using AttributeValue =
    nostd::variant<bool,
                   int32_t,
                   int64_t,
                   uint32_t,
                   uint64_t,
                   double,
                   nostd::string_view,
#ifdef HAVE_SPAN_BYTE
                   // TODO: 8-bit byte arrays / binary blobs are not part of OT spec yet!
                   // Ref: https://github.com/open-telemetry/opentelemetry-specification/issues/780
                   nostd::span<const uint8_t>,
#endif
                   nostd::span<const bool>,
                   nostd::span<const int32_t>,
                   nostd::span<const int64_t>,
                   nostd::span<const uint32_t>,
                   nostd::span<const uint64_t>,
                   nostd::span<const double>,
                   nostd::span<const nostd::string_view>>;

enum AttributeType
{
  kTypeBool,
  kTypeInt,
  kTypeInt64,
  kTypeUInt,
  kTypeUInt64,
  kTypeDouble,
  kTypeString,
#ifdef HAVE_SPAN_BYTE
  kTypeSpanByte,
#endif
  kTypeSpanBool,
  kTypeSpanInt,
  kTypeSpanInt64,
  kTypeSpanUInt,
  kTypeSpanUInt64,
  kTypeSpanDouble,
  kTypeSpanString
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
