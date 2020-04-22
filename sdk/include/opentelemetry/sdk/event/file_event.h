#pragma once

#include <cstdint>
#include <functional>
#include <system_error>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace event
{
/**
 * Available events that can be monitored on a file descriptor.
 */
struct FileReadyType
{
  static const uint32_t kRead   = 1;
  static const uint32_t kWrite  = (1 << 1);
  static const uint32_t kClosed = (1 << 2);
};

using FileReadyCallback = std::function<void(uint32_t)>;

/**
 * Manages an event set on a file descriptor.
 *
 * Note: Must be freed before the dipatcher is destructed
 */
class FileEvent
{
public:
  virtual ~FileEvent() = default;
};
}  // namespace event
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
