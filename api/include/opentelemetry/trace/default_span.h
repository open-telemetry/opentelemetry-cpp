// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class DefaultSpan : public Span
{
public:
  // Returns an invalid span.
  static DefaultSpan GetInvalid() { return DefaultSpan(SpanContext::GetInvalid()); }

  trace::SpanContext GetContext() const noexcept { return span_context_; }

  bool IsRecording() const noexcept { return false; }

  void SetAttribute(nostd::string_view /* key */,
                    const common::AttributeValue & /* value */) noexcept
  {}

  void AddEvent(nostd::string_view /* name */) noexcept {}

  void AddEvent(nostd::string_view /* name */, common::SystemTimestamp /* timestamp */) noexcept {}

  void AddEvent(nostd::string_view /* name */,
                common::SystemTimestamp /* timestamp */,
                const common::KeyValueIterable & /* attributes */) noexcept
  {}

  void AddEvent(nostd::string_view name, const common::KeyValueIterable &attributes) noexcept
  {
    this->AddEvent(name, std::chrono::system_clock::now(), attributes);
  }

  void SetStatus(StatusCode /* status */, nostd::string_view /* description */) noexcept {}

  void UpdateName(nostd::string_view /* name */) noexcept {}

  void End(const EndSpanOptions & /* options */ = {}) noexcept {}

  nostd::string_view ToString() { return "DefaultSpan"; }

  DefaultSpan(SpanContext span_context) : span_context_(span_context) {}

  // movable and copiable
  DefaultSpan(DefaultSpan &&spn) : span_context_(spn.GetContext()) {}
  DefaultSpan(const DefaultSpan &spn) : span_context_(spn.GetContext()) {}

private:
  SpanContext span_context_;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
