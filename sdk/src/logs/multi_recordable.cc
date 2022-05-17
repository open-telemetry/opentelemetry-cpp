// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/multi_recordable.h"

#  include <cstddef>
#  include <memory>
#  include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

namespace
{
std::size_t MakeKey(const opentelemetry::sdk::logs::LogProcessor &processor)
{
  return reinterpret_cast<std::size_t>(&processor);
}

}  // namespace

void MultiRecordable::AddRecordable(const LogProcessor &processor,
                                    std::unique_ptr<Recordable> recordable) noexcept
{
  recordables_[MakeKey(processor)] = std::move(recordable);
}

const std::unique_ptr<Recordable> &MultiRecordable::GetRecordable(
    const LogProcessor &processor) const noexcept
{
  // TODO - return nullptr ref on failed lookup?
  auto i = recordables_.find(MakeKey(processor));
  if (i != recordables_.end())
  {
    return i->second;
  }
  static std::unique_ptr<Recordable> empty(nullptr);
  return empty;
}

std::unique_ptr<Recordable> MultiRecordable::ReleaseRecordable(
    const LogProcessor &processor) noexcept
{
  auto i = recordables_.find(MakeKey(processor));
  if (i != recordables_.end())
  {
    std::unique_ptr<Recordable> result(i->second.release());
    recordables_.erase(MakeKey(processor));
    return result;
  }
  return std::unique_ptr<Recordable>(nullptr);
}

void MultiRecordable::SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetTimestamp(timestamp);
  }
}

void MultiRecordable::SetSeverity(opentelemetry::logs::Severity severity) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetSeverity(severity);
  }
}

void MultiRecordable::SetBody(nostd::string_view message) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetBody(message);
  }
}

void MultiRecordable::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetResource(resource);
  }
}

void MultiRecordable::SetAttribute(nostd::string_view key,
                                   const opentelemetry::common::AttributeValue &value) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetAttribute(key, value);
  }
}

void MultiRecordable::SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetTraceId(trace_id);
  }
}

void MultiRecordable::SetSpanId(opentelemetry::trace::SpanId span_id) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetSpanId(span_id);
  }
}

void MultiRecordable::SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept
{
  for (auto &recordable : recordables_)
  {
    recordable.second->SetTraceFlags(trace_flags);
  }
}

void MultiRecordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{
  instrumentation_library_ = &instrumentation_library;
}

const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &
MultiRecordable::GetInstrumentationLibrary() const noexcept
{
  return *instrumentation_library_;
}
}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
