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
class Span final : public opentelemetry::trace::Span
{
public:
  Span(std::shared_ptr<Tracer> &&tracer,
       nostd::string_view name,
       const opentelemetry::common::KeyValueIterable &attributes,
       const opentelemetry::trace::SpanContextKeyValueIterable &links,
       const opentelemetry::trace::StartSpanOptions &options,
       const opentelemetry::trace::SpanContext &parent_span_context,
       std::unique_ptr<opentelemetry::trace::SpanContext> span_context) noexcept;

  ~Span() override;

  // opentelemetry::trace::Span
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name) noexcept override;

  void AddEvent(nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  void AddEvent(nostd::string_view name,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void AddEvent(nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void AddLink(const opentelemetry::trace::SpanContext &target,
               const opentelemetry::common::KeyValueIterable &attrs) noexcept override;

  void AddLinks(const opentelemetry::trace::SpanContextKeyValueIterable &links) noexcept override;
#endif

  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description) noexcept override;

  void UpdateName(nostd::string_view name) noexcept override;

  void End(const opentelemetry::trace::EndSpanOptions &options = {}) noexcept override;

  bool IsRecording() const noexcept override;

  opentelemetry::trace::SpanContext GetContext() const noexcept override
  {
    return *span_context_.get();
  }

private:
  std::shared_ptr<Tracer> tracer_;
  mutable std::mutex mu_;
  std::unique_ptr<Recordable> recordable_;
  opentelemetry::common::SteadyTimestamp start_steady_time;
  std::unique_ptr<opentelemetry::trace::SpanContext> span_context_;
  bool has_ended_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
