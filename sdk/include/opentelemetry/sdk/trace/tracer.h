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

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

using namespace opentelemetry::sdk::instrumentationlibrary;

class Tracer final : public trace_api::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  /** Construct a new Tracer with the given context pipeline. */
  explicit Tracer(std::shared_ptr<sdk::trace::TracerContext> context,
                  std::unique_ptr<InstrumentationLibrary> instrumentation_library =
                      InstrumentationLibrary::create("")) noexcept;

  nostd::shared_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const opentelemetry::common::KeyValueIterable &attributes,
      const trace_api::SpanContextKeyValueIterable &links,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;

  /** Returns the configured span processor. */
  SpanProcessor &GetProcessor() noexcept { return context_->GetProcessor(); }

  /** Returns the configured Id generator */
  IdGenerator &GetIdGenerator() const noexcept { return context_->GetIdGenerator(); }

  /** Returns the associated instruementation library */
  const InstrumentationLibrary &GetInstrumentationLibrary() const noexcept
  {
    return *instrumentation_library_;
  }

  /** Returns the currently configured resource **/
  const opentelemetry::sdk::resource::Resource &GetResource() { return context_->GetResource(); }

  // Note: Test only
  Sampler &GetSampler() { return context_->GetSampler(); }

private:
  std::shared_ptr<sdk::trace::TracerContext> context_;
  std::shared_ptr<InstrumentationLibrary> instrumentation_library_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
