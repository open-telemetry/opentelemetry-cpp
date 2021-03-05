// Copyright 2020, OpenTelemetry Authors
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

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
/**
 * Controls how long a span is active.
 *
 * On creation of the Scope object, the given span is set to the currently
 * active span. On destruction, the given span is ended and the previously
 * active span will be the currently active span again.
 */
class Scope final
{
public:
  /**
   * Initialize a new scope.
   * @param span the given span will be set as the currently active span.
   */
  Scope(const nostd::shared_ptr<Span> &span) noexcept
      : token_(context::RuntimeContext::Attach(
            context::RuntimeContext::GetCurrent().SetValue(kSpanKey, span)))
  {}

private:
  nostd::unique_ptr<context::Token> token_;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
