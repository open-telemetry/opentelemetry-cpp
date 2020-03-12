#pragma once

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"
#include "src/trace/recorder.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Tracer final : public trace_api::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  // Note: recorder must be non-null
  explicit Tracer(std::unique_ptr<Recorder> &&recorder) noexcept : recorder_{std::move(recorder)} {}

  Recorder &recorder() const noexcept { return *recorder_; }

  // trace_api::Tracer
  nostd::unique_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

private:
  std::unique_ptr<Recorder> recorder_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
