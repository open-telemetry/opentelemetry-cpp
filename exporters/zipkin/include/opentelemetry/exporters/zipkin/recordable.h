/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "nlohmann/json.hpp"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{
using ZipkinSpan = nlohmann::json;

enum class TransportFormat
{
  kJson,
  kProtobuf
};

class Recordable final : public sdk::trace::Recordable
{
public:
  const ZipkinSpan &span() const noexcept { return span_; }

  const std::string &GetServiceName() const noexcept { return service_name_; }

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name,
                common::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace::StatusCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetStartTime(opentelemetry::common::SystemTimestamp start_time) noexcept override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;

  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

  void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept override;

private:
  ZipkinSpan span_;
  std::string service_name_;
};
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
