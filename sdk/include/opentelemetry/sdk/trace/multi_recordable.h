#pragma once

#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

#include <map>

// TODO: Create generic short pattern for opentelemetry::common and opentelemetry::trace

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace
{
std::size_t MakeKey(const SpanProcessor &processor)
{
  return reinterpret_cast<std::size_t>(&processor);
}

}  // namespace

/**
 * Maintains a representation of a span in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */
class MultiRecordable : public Recordable
{
public:
  void AddRecordable(const SpanProcessor &processor,
                     std::unique_ptr<Recordable> recordable) noexcept
  {
    recordables_[MakeKey(processor)] = std::move(recordable);
  }

  const std::unique_ptr<Recordable> &GetRecordable(const SpanProcessor &processor) const noexcept
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

  std::unique_ptr<Recordable> ReleaseRecordable(const SpanProcessor &processor) noexcept
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

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetIdentity(span_context, parent_span_id);
    }
  }

  virtual void SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetAttribute(key, value);
    }
  }

  virtual void AddEvent(nostd::string_view name,
                        core::SystemTimestamp timestamp,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {

    for (auto &recordable : recordables_)
    {
      recordable.second->AddEvent(name, timestamp, attributes);
    }
  }

  virtual void AddLink(const opentelemetry::trace::SpanContext &span_context,
                       const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->AddLink(span_context, attributes);
    }
  }

  virtual void SetStatus(opentelemetry::trace::StatusCode code,
                         nostd::string_view description) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetStatus(code, description);
    }
  }

  virtual void SetName(nostd::string_view name) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetName(name);
    }
  }

  virtual void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetSpanKind(span_kind);
    }
  }

  virtual void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetStartTime(start_time);
    }
  }

  virtual void SetDuration(std::chrono::nanoseconds duration) noexcept override
  {
    for (auto &recordable : recordables_)
    {
      recordable.second->SetDuration(duration);
    }
  }

private:
  std::map<std::size_t, std::unique_ptr<Recordable>> recordables_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE