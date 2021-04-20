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
#include "opentelemetry/sdk/trace/id_generator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class RandomIdGenerator : public IdGenerator
{
public:
  opentelemetry::trace::SpanId GenerateSpanId() noexcept override;

  opentelemetry::trace::TraceId GenerateTraceId() noexcept override;
};

}  // namespace trace

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE