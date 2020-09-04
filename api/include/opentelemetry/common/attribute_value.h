#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
using AttributeValue = nostd::variant<bool,
                                      int32_t,
                                      int64_t,
                                      uint32_t,
                                      uint64_t,
                                      double,
                                      nostd::string_view,
                                      const char*,
#ifdef HAVE_SPAN_BYTE
                                      nostd::span<const uint8_t>,   // TODO: not part of OT spec yet
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
  TYPE_CSTRING,
#if HAVE_SPAN_BYTE
  TYPE_SPAN_BYTE,   // TODO: not part of OT spec yet
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
