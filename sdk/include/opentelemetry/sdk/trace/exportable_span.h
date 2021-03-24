#pragma once

#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class SpanExporter;

/**
 * A representation of only information used to export spans.
 *
 * Unlike `sdk::Span`, this simply tracks the originating recorable + Tracer which generated
 * a span.  Additionally it delegates Recordable calls to underlying recordadble interfaces.
 */
class ExportableSpan : public Recordable
{
public:
  explicit ExportableSpan(std::shared_ptr<Tracer> tracer);
  virtual ~ExportableSpan();


  /** 
   * Constructs a new unique Exportable span which extracts any exportable associated
   * with a given processor.
   * 
   * Note: This is used to coordinate exportable span in a mulit-span processor.
   */
  std::unique_ptr<ExportableSpan> ReleaseExportableSpanFor(const SpanProcessor& processor) noexcept;
  /**
   * Registers a recordable for a given processor that this span should write data into.
   */
  void RegisterRecordableFor(const SpanProcessor& processor, std::unique_ptr<Recordable> recordable) noexcept;
  /**
   * Releases ownership of the originally registered recordable.
   */
  std::unique_ptr<Recordable> ReleaseRecordableFor(const SpanProcessor& processor) noexcept;
  /** Access the recordable attached by a given processor. */
  const std::unique_ptr<Recordable>& GetRecordableFor(const SpanProcessor& processor) const noexcept;


  Tracer &GetTracer() { return *tracer_; }
  // Note: used in MultiSpanProcessor, clean this up...
  std::shared_ptr<Tracer> ShareTracer() { return tracer_; }

  // Recordable Interface
  void SetIds(opentelemetry::trace::TraceId trace_id,
              opentelemetry::trace::SpanId span_id,
              opentelemetry::trace::SpanId parent_span_id) noexcept override;
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;
  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;
  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const opentelemetry::common::KeyValueIterable &attributes) noexcept override;
  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description) noexcept override;
  void SetName(nostd::string_view name) noexcept override;
  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;
  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override;
  void SetDuration(std::chrono::nanoseconds duration) noexcept override;
private:
  std::shared_ptr<Tracer> tracer_;
  // TODO - more efficient data structure
  std::map<std::size_t, std::unique_ptr<Recordable>> recordables_;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE