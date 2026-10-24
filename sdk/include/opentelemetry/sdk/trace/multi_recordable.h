// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <map>
#include <memory>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class MultiRecordable : public Recordable
{
  static std::size_t MakeKey(const SpanProcessor &processor);

public:
  void AddRecordable(const SpanProcessor &processor, std::unique_ptr<Recordable> recordable);

  const std::unique_ptr<Recordable> &GetRecordable(const SpanProcessor &processor) const;

  std::unique_ptr<Recordable> ReleaseRecordable(const SpanProcessor &processor);

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) override;

  void AddEvent(nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const opentelemetry::common::KeyValueIterable &attributes) override;

  void SetStatus(opentelemetry::trace::StatusCode code, nostd::string_view description) override;

  void SetName(nostd::string_view name) override;

  void SetTraceFlags(opentelemetry::trace::TraceFlags flags) override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) override;

  void SetResource(const opentelemetry::sdk::resource::Resource &resource) override;

  void SetStartTime(opentelemetry::common::SystemTimestamp start_time) override;

  void SetDuration(std::chrono::nanoseconds duration) override;

  void SetSpanLimits(const SpanLimits &limits) override;

  void SetInstrumentationScope(const InstrumentationScope &instrumentation_scope) override;

private:
  std::map<std::size_t, std::unique_ptr<Recordable>> recordables_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
