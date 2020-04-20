#include <cstdint>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace event
{

// TODO: bitset instead?
typedef uint64_t Attributes;

struct Attribute
{
  uint64_t value;
  static const uint64_t ATTRIB_NONE       = 0;          // 0
  static const uint64_t ATTRIB_EUII_ID    = 1;          // 1
  static const uint64_t ATTRIB_EUII_IPv4  = 1 << 1;     // 2
  static const uint64_t ATTRIB_EUII_IPv6  = 1 << 2;     // 3
  static const uint64_t ATTRIB_EUII_Smtp  = 1 << 3;     // 4
  static const uint64_t ATTRIB_EUII_Phone = 1 << 4;     // 5
  static const uint64_t ATTRIB_EUII_Uri   = 1 << 5;     // 6
  static const uint64_t ATTRIB_EUII_7     = 1 << 6;     // 7
  static const uint64_t ATTRIB_EUII_8     = 1 << 7;     // 8
  static const uint64_t ATTRIB_EUII_9     = 1 << 8;     // 9
  Attribute(uint64_t rvalue) : value(rvalue){};
  operator uint64_t() const { return value; };
};

}  // namespace event

OPENTELEMETRY_END_NAMESPACE
