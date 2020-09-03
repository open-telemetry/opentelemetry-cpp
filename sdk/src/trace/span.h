#pragma once

#include <mutex>

#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

class Span final : public trace_api::Span
{
public:
  explicit Span(std::shared_ptr<Tracer> &&tracer,
                std::shared_ptr<SpanProcessor> processor,
                nostd::string_view name,
                const trace_api::KeyValueIterable &attributes,
                const trace_api::StartSpanOptions &options,
                const trace_api::SpanContext &parent_span_context) noexcept;

  ~Span() override;

  // trace_api::Span
  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name) noexcept override;

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override;

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace_api::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept override;

  void UpdateName(nostd::string_view name) noexcept override;

  void End(const trace_api::EndSpanOptions &options = {}) noexcept override;

  bool IsRecording() const noexcept override;

  trace_api::SpanContext GetContext() const noexcept override { return *span_context_.get(); }

  void SetToken(nostd::unique_ptr<context::Token> &&token) noexcept override;

private:
  std::shared_ptr<trace_api::Tracer> tracer_;
  std::shared_ptr<SpanProcessor> processor_;
  mutable std::mutex mu_;
  std::unique_ptr<Recordable> recordable_;
  opentelemetry::core::SteadyTimestamp start_steady_time;
  std::unique_ptr<trace_api::SpanContext> span_context_;
  bool has_ended_;
  nostd::unique_ptr<context::Token> token_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
