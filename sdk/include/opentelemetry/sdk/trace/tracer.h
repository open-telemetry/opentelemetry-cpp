// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
                      InstrumentationLibrary::Create("")) noexcept;

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
  // order of declaration is important here - instrumentation library should destroy after
  // tracer-context.
  std::shared_ptr<InstrumentationLibrary> instrumentation_library_;
  std::shared_ptr<sdk::trace::TracerContext> context_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
