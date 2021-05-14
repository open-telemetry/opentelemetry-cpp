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
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class DefaultTracer : public Tracer
{
public:
  ~DefaultTracer() = default;

  /**
   * Starts a span.
   *
   * Optionally sets attributes at Span creation from the given key/value pairs.
   *
   * Attributes will be processed in order, previous attributes with the same
   * key will be overwritten.
   */
  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const common::KeyValueIterable &attributes,
                                    const StartSpanOptions &options = {}) override noexcept
  {
    return nostd::unique_ptr<Span>(new DefaultSpan::GetInvalid());
  }

  void ForceFlushWithMicroseconds(uint64_t timeout) override noexcept {}

  void CloseWithMicroseconds(uint64_t timeout) override noexcept {}
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
