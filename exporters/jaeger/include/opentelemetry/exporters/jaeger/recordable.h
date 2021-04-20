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

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view key,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace::StatusCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

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
  std::vector<thrift::Tag> tags_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
