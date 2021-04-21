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
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/** IdGenerator provides an interface for generating Trace Id and Span Id */
class IdGenerator
{

public:
  virtual ~IdGenerator() = default;

  /** Returns a SpanId represented by opaque 128-bit trace identifier */
  virtual opentelemetry::trace::SpanId GenerateSpanId() noexcept = 0;

  /** Returns a TraceId represented by opaque 64-bit trace identifier */
  virtual opentelemetry::trace::TraceId GenerateTraceId() noexcept = 0;
};
}  // namespace trace

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE