#pragma once

#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "src/trace/recordable.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
class Recorder
{
public:
  virtual ~Recorder() = default;

  /**
   * @return a Recordable object to maintain a data representation of a span.
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * Record a span.
   * @param recordable the data representation of the span.
   */
  virtual void Record(std::unique_ptr<Recordable> &&recordable) noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
