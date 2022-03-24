// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "src/trace/span.h"
#include "src/common/random.h"

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

using opentelemetry::common::SteadyTimestamp;
using opentelemetry::common::SystemTimestamp;
namespace common = opentelemetry::common;

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
           nostd::string_view name,
           const common::KeyValueIterable &attributes,
           const trace_api::SpanContextKeyValueIterable &links,
           const trace_api::StartSpanOptions &options,
           const trace_api::SpanContext &parent_span_context,
           std::unique_ptr<trace_api::SpanContext> span_context) noexcept
    : tracer_{std::move(tracer)},
      recordable_{tracer_->GetProcessor().MakeRecordable()},
      start_steady_time{options.start_steady_time},
      span_context_(std::move(span_context)),
      has_ended_{false}
{
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->SetName(name);
  recordable_->SetInstrumentationLibrary(tracer_->GetInstrumentationLibrary());
  recordable_->SetIdentity(*span_context_, parent_span_context.IsValid()
                                               ? parent_span_context.span_id()
                                               : trace_api::SpanId());

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    recordable_->SetAttribute(key, value);
    return true;
  });

  links.ForEachKeyValue([&](opentelemetry::trace::SpanContext span_context,
                            const common::KeyValueIterable &attributes) {
    recordable_->AddLink(span_context, attributes);
    return true;
  });

  recordable_->SetSpanKind(options.kind);
  recordable_->SetStartTime(NowOr(options.start_system_time));
  start_steady_time = NowOr(options.start_steady_time);
  recordable_->SetResource(tracer_->GetResource());
  tracer_->GetProcessor().OnStart(*recordable_, parent_span_context);
}

Span::~Span()
{
  End();
}

void Span::SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }

  recordable_->SetAttribute(key, value);
}

void Span::AddEvent(nostd::string_view name) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->AddEvent(name);
}

void Span::AddEvent(nostd::string_view name, SystemTimestamp timestamp) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->AddEvent(name, timestamp);
}

void Span::AddEvent(nostd::string_view name,
                    SystemTimestamp timestamp,
                    const common::KeyValueIterable &attributes) noexcept
{
  std::lock_guard<std::mutex> lock_guard{mu_};
  if (recordable_ == nullptr)
  {
    return;
  }
  recordable_->AddEvent(name, timestamp, attributes);
}

void Span::SetStatus(opentelemetry::trace::StatusCode code, nostd::string_view description) noexcept
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

  if (has_ended_ == true)
  {
    return;
  }
  has_ended_ = true;

  if (recordable_ == nullptr)
  {
    return;
  }

  auto end_steady_time = NowOr(options.end_steady_time);
  recordable_->SetDuration(std::chrono::steady_clock::time_point(end_steady_time) -
                           std::chrono::steady_clock::time_point(start_steady_time));

  tracer_->GetProcessor().OnEnd(std::move(recordable_));
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
