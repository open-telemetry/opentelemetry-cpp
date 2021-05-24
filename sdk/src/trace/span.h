// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
  Span(std::shared_ptr<Tracer> &&tracer,
       nostd::string_view name,
       const opentelemetry::common::KeyValueIterable &attributes,
       const trace_api::SpanContextKeyValueIterable &links,
       const trace_api::StartSpanOptions &options,
       const trace_api::SpanContext &parent_span_context,
       std::unique_ptr<trace_api::SpanContext> span_context) noexcept;

  ~Span() override;

  // trace_api::Span
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name) noexcept override;

  void AddEvent(nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  void AddEvent(nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace_api::StatusCode code, nostd::string_view description) noexcept override;

  void UpdateName(nostd::string_view name) noexcept override;

  void End(const trace_api::EndSpanOptions &options = {}) noexcept override;

  bool IsRecording() const noexcept override;

  trace_api::SpanContext GetContext() const noexcept override { return *span_context_.get(); }

private:
  std::shared_ptr<Tracer> tracer_;
  mutable std::mutex mu_;
  std::unique_ptr<Recordable> recordable_;
  opentelemetry::common::SteadyTimestamp start_steady_time;
  std::unique_ptr<trace_api::SpanContext> span_context_;
  bool has_ended_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
