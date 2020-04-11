#pragma once

#include <chrono>
#include <system_error>
#include <cstdint>
#include <functional>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk {
namespace event {
struct FileReadyType {
  static const uint32_t kRead = 1;
  static const uint32_t kWrite = 2;
  static const uint32_t kClosed = 3;
  static const uint32_t kTimeout = 4;
};

using FileReadyCallback = std::function<void(uint32_t)>;

class FileEvent {
 public:
   virtual ~FileEvent() = default;

};
} // namespace event
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
