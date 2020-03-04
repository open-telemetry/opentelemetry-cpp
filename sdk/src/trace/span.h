#pragma once

#include "src/trace/tracer.h"

#include <mutex>

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

class Span final : public trace_api::Span
{
public:
  explicit Span(std::shared_ptr<Tracer> &&tracer,
                nostd::string_view name,
                const trace_api::StartSpanOptions &options) noexcept;

  ~Span() override;

  void AddEvent(nostd::string_view name) noexcept override;

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override;
  void AddEvent(nostd::string_view name, core::SteadyTimestamp timestamp) noexcept override;

  void SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept override;

  void UpdateName(nostd::string_view name) noexcept override;

  void End() noexcept override;

  bool IsRecording() const noexcept override;

  trace_api::Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<Tracer> tracer_;
  mutable std::mutex mu_;
  std::unique_ptr<Recordable> recordable_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
