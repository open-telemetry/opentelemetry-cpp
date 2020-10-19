#pragma once

#include <memory>

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/plugin/detail/dynamic_library_handle.h"
#include "opentelemetry/plugin/detail/tracer_handle.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
class Span final : public trace::Span
{
public:
  Span(std::shared_ptr<trace::Tracer> &&tracer, nostd::shared_ptr<trace::Span> span) noexcept
      : tracer_{std::move(tracer)}, span_{span}
  {}

  // trace::Span
  void SetAttribute(nostd::string_view name, const common::AttributeValue &value) noexcept override
  {
    span_->SetAttribute(name, value);
  }

  void AddEvent(nostd::string_view name) noexcept override { span_->AddEvent(name); }

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override
  {
    span_->AddEvent(name, timestamp);
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override
  {
    span_->AddEvent(name, timestamp, attributes);
  }

  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override
  {
    span_->SetStatus(code, description);
  }

  void UpdateName(nostd::string_view name) noexcept override { span_->UpdateName(name); }

  void End(const trace::EndSpanOptions &options = {}) noexcept override { span_->End(options); }

  bool IsRecording() const noexcept override { return span_->IsRecording(); }

  trace::SpanContext GetContext() const noexcept override { return span_->GetContext(); }

private:
  std::shared_ptr<trace::Tracer> tracer_;
  nostd::shared_ptr<trace::Span> span_;
};

class Tracer final : public trace::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  Tracer(std::shared_ptr<DynamicLibraryHandle> library_handle,
         std::unique_ptr<TracerHandle> &&tracer_handle) noexcept
      : library_handle_{std::move(library_handle)}, tracer_handle_{std::move(tracer_handle)}
  {}

  // trace::Tracer
  nostd::shared_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const common::KeyValueIterable &attributes,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    auto span = tracer_handle_->tracer().StartSpan(name, attributes, options);
    if (span == nullptr)
    {
      return nostd::shared_ptr<trace::Span>(nullptr);
    }
    return nostd::shared_ptr<trace::Span>{new (std::nothrow) Span{this->shared_from_this(), span}};
  }

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override
  {
    tracer_handle_->tracer().ForceFlushWithMicroseconds(timeout);
  }

  void CloseWithMicroseconds(uint64_t timeout) noexcept override
  {
    tracer_handle_->tracer().CloseWithMicroseconds(timeout);
  }

private:
  // Note: The order is important here.
  //
  // It's undefined behavior to close the library while a loaded tracer is still active.
  std::shared_ptr<DynamicLibraryHandle> library_handle_;
  std::unique_ptr<TracerHandle> tracer_handle_;
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
