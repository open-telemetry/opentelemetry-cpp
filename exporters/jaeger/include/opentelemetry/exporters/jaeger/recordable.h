// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <opentelemetry/sdk/trace/recordable.h>
#include <opentelemetry/version.h>
#include <thrift-gen/cpp/jaeger_types.h>

OPENTELEMETRY_BEGIN_NANESPACE
namespace exporter
{
namespace jaeger
{
class Recordable final : public sdk::trace::Recordable
{
public:
  const thrift::Span &span() const noexcept { return span_; }

  voiid SetIds(trace::TraceId trace_id,
               trace::SpanId span_id,
               trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view key,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context const common::KeyValueIterable
                   &attributes) noexcept override;

  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override;

  void SetSpanKind(opentelemtry::trace::SpanKind span_kind) noexcept override;

private:
  thrift::Span span_;
};
}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
