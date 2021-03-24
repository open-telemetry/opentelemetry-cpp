#include "opentelemetry/sdk/trace/exportable_span.h"

#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

namespace 
{

std::size_t MakeKey(const SpanProcessor& processor)
{
    return reinterpret_cast<std::size_t>(&processor);
}

}  // namespace

ExportableSpan::ExportableSpan(std::shared_ptr<Tracer> tracer) 
 : tracer_(std::move(tracer)) {}
ExportableSpan::~ExportableSpan() {}


std::unique_ptr<ExportableSpan> ExportableSpan::ReleaseExportableSpanFor(const SpanProcessor& processor) noexcept
{
    std::unique_ptr<ExportableSpan> view(new ExportableSpan(tracer_));
    auto recordable = ReleaseRecordableFor(processor);
    if (recordable != nullptr)
    {
        view->RegisterRecordableFor(processor, std::move(recordable));
    }
    return view;
}

std::unique_ptr<Recordable> ExportableSpan::ReleaseRecordableFor(const SpanProcessor& processor) noexcept
{
    auto i = recordables_.find(MakeKey(processor));
    if (i != recordables_.end()) {
        std::unique_ptr<Recordable> result(i->second.release());
        recordables_.erase(MakeKey(processor));
        return result;
    }
    return std::unique_ptr<Recordable>(nullptr);
}

const std::unique_ptr<Recordable>& ExportableSpan::GetRecordableFor(const SpanProcessor& processor) const noexcept
{
    // TODO - return nullptr ref on failed lookup?
    static std::unique_ptr<Recordable> empty(nullptr);
    auto i = recordables_.find(MakeKey(processor));
    if (i != recordables_.end())
    {
        return i->second;
    }
    return empty;
}

void ExportableSpan::RegisterRecordableFor(const SpanProcessor& processor, std::unique_ptr<Recordable> recordable) noexcept 
{
    // TODO
    recordables_[MakeKey(processor)] = std::move(recordable);
}


void ExportableSpan::SetIds(opentelemetry::trace::TraceId trace_id,
            opentelemetry::trace::SpanId span_id,
            opentelemetry::trace::SpanId parent_span_id) noexcept {
    for (auto& recordable : recordables_) 
    {
        recordable.second->SetIds(trace_id, span_id, parent_span_id);
    }
}
void ExportableSpan::SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
{
    for (auto& recordable : recordables_) 
    {
        recordable.second->SetAttribute(key, value);
    }
}
void ExportableSpan::AddEvent(nostd::string_view name,
            core::SystemTimestamp timestamp,
            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
    for (auto& recordable : recordables_) 
    {
        recordable.second->AddEvent(name, timestamp, attributes);
    }
}
void ExportableSpan::AddLink(const opentelemetry::trace::SpanContext &span_context,
                       const opentelemetry::common::KeyValueIterable &attributes) noexcept 
{
  for (auto& recordable : recordables_) 
    {
        recordable.second->AddLink(span_context, attributes);
    }
}
void ExportableSpan::SetStatus(opentelemetry::trace::StatusCode code,
                nostd::string_view description) noexcept
{
    for (auto& recordable : recordables_) 
    {
         recordable.second->SetStatus(code, description);
    }
}
void ExportableSpan::SetName(nostd::string_view name) noexcept
{
    for (auto& recordable : recordables_) 
    {
         recordable.second->SetName(name);
    }
}
void ExportableSpan::SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept
{
    for (auto& recordable : recordables_) 
    {
         recordable.second->SetSpanKind(span_kind);
    }
}
void ExportableSpan::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
    for (auto& recordable : recordables_) 
    {
         recordable.second->SetStartTime(start_time);
    }
}

void ExportableSpan::SetDuration(std::chrono::nanoseconds duration) noexcept
{
    for (auto& recordable : recordables_) 
    {
        recordable.second->SetDuration(duration);
    }
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
