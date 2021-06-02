// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <jaeger_types.h>
#include <opentelemetry/sdk/trace/recordable.h>
#include <opentelemetry/version.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

class Recordable final : public sdk::trace::Recordable
{
public:
  Recordable();

  thrift::Span *Span() noexcept { return span_.release(); }
  std::vector<thrift::Tag> Tags() noexcept { return std::move(tags_); }
  const std::string &ServiceName() const noexcept { return service_name_; }

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view key,
                common::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace::StatusCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetStartTime(common::SystemTimestamp start_time) noexcept override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;

  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

  void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept override;

  void SetSpanLimits(const opentelemetry::sdk::trace::SpanLimits &span_limits) noexcept override;

private:
  void AddTag(const std::string &key, const std::string &value);
  void AddTag(const std::string &key, const char *value);
  void AddTag(const std::string &key, bool value);
  void AddTag(const std::string &key, int64_t value);
  void AddTag(const std::string &key, double value);

  void PopulateAttribute(nostd::string_view key,
                         const opentelemetry::common::AttributeValue &value);

private:
  std::unique_ptr<thrift::Span> span_;
  opentelemetry::sdk::trace::SpanLimits span_limit_;
  std::vector<thrift::Tag> tags_;
  std::string service_name_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
