#pragma once

#include <mutex>

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/sdk/trace/exportable_span.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class Span final : public opentelemetry::trace::Span, public std::enable_shared_from_this<Span>
{
public:
  explicit Span(std::shared_ptr<Tracer> &&tracer,
                nostd::string_view name,
                const opentelemetry::common::KeyValueIterable &attributes,
                const opentelemetry::trace::SpanContextKeyValueIterable &links,
                const opentelemetry::trace::StartSpanOptions &options,
                const opentelemetry::trace::SpanContext &parent_span_context) noexcept;

  ~Span() override;

  // trace_api::Span
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name) noexcept override;

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override;

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(opentelemetry::trace::StatusCode code, nostd::string_view description) noexcept override;

  void UpdateName(nostd::string_view name) noexcept override;

  void End(const opentelemetry::trace::EndSpanOptions &options = {}) noexcept override;

  bool IsRecording() const noexcept override;

  trace_api::SpanContext GetContext() const noexcept override { return *span_context_.get(); }

private:
  // Returns the recordable, or nullptr if not available.
  Recordable* GetRecordable() const {
    if (exportable_ != nullptr) {
      return exportable_.get();
    }
    return nullptr;
  }
  mutable std::mutex mu_;
  std::unique_ptr<ExportableSpan> exportable_;
  opentelemetry::core::SteadyTimestamp start_steady_time;
  std::unique_ptr<opentelemetry::trace::SpanContext> span_context_;
  bool has_ended_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
