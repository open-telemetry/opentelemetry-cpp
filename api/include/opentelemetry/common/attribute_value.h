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
#ifdef HAVE_CSTRING_TYPE
                   // TODO: add C-string as possible value on API surface
                   const char *,
#endif
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
  TYPE_BOOL,
  TYPE_INT,
  TYPE_INT64,
  TYPE_UINT,
  TYPE_UINT64,
  TYPE_DOUBLE,
  TYPE_STRING,
#ifdef HAVE_CSTRING_TYPE
  TYPE_CSTRING,
#endif
#ifdef HAVE_SPAN_BYTE
  TYPE_SPAN_BYTE,
#endif
  TYPE_SPAN_BOOL,
  TYPE_SPAN_INT,
  TYPE_SPAN_INT64,
  TYPE_SPAN_UINT,
  TYPE_SPAN_UINT64,
  TYPE_SPAN_DOUBLE,
  TYPE_SPAN_STRING
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
