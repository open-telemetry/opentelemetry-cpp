#pragma once

#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/span_id.h"
#include "src/trace/recordable.h"

namespace opentelemetry
{
namespace sdk {
namespace trace
{
class Recorder {
 public:
   virtual ~Recorder() = default;

   virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

   virtual void Record(std::unique_ptr<Recordable>&& recordable) noexcept = 0;
};
}  // namespace trace
} // namespace sdk
}  // namespace opentelemetry
