#include "src/trace/span.h"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

using opentelemetry::core::SteadyTimestamp;
using opentelemetry::core::SystemTimestamp;

namespace
{
SystemTimestamp NowOr(const SystemTimestamp &system)
{
  if (system == SystemTimestamp())
  {
    return SystemTimestamp(std::chrono::system_clock::now());
  }
  else
  {
    return system;
  }
}

SteadyTimestamp NowOr(const SteadyTimestamp &steady)
{
  if (steady == SteadyTimestamp())
  {
    return SteadyTimestamp(std::chrono::steady_clock::now());
  }
  else
  {
    return steady;
  }
}
}  // namespace

Span::Span(std::shared_ptr<Tracer> &&tracer,
           std::shared_ptr<SpanProcessor> processor,
           nostd::string_view name,
           const trace_api::StartSpanOptions &options) noexcept
    : tracer_{std::move(tracer)},
      processor_{processor},
      recordable_{processor_->MakeRecordable()},
      start_steady_time{options.start_steady_time}
{
  (void)options;
  if (recordable_ == nullptr)
  {
    return;
  }
  processor_->OnStart(*recordable_);
  recordable_->SetName(name);

  recordable_->SetStartTime(NowOr(options.start_system_time));
  start_steady_time = NowOr(options.start_steady_time);
}

Span::~Span()
{
  End();
}

void Span::AddEvent(nostd::string_view name) noexcept
{
  (void)name;
}

void Span::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  (void)name;
  (void)timestamp;
}

void Span::AddEvent(nostd::string_view name,
                    core::SystemTimestamp timestamp,
                    const trace_api::KeyValueIterable &attributes) noexcept
{
  (void)name;
  (void)timestamp;
  (void)attributes;
}

void Span::SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->SetStatus(code, description);
}

void Span::UpdateName(nostd::string_view name) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->SetName(name);
}

void Span::End(const trace_api::EndSpanOptions &options) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }

  auto end_steady_time = NowOr(options.end_steady_time);
  recordable_->SetDuration(std::chrono::steady_clock::time_point(end_steady_time) -
                           std::chrono::steady_clock::time_point(start_steady_time));

  processor_->OnEnd(std::move(recordable_));
  recordable_.reset();
}

bool Span::IsRecording() const noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  return recordable_ != nullptr;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
