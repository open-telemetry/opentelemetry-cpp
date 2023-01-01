/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentracing/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim 
{

using TracerPtr = nostd::shared_ptr<opentelemetry::trace::Tracer>;
using TracerProviderPtr = nostd::shared_ptr<opentelemetry::trace::TracerProvider>;
using PropagatorPtr = nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator>;

struct OpenTracingPropagators {
    PropagatorPtr textMap;
    PropagatorPtr httpHeaders;
};

class TracerShim : public opentracing::Tracer
{
public:

  /**
   * Creates a {@code opentracing::Tracer} shim out of 
   * {@code Provider::GetTracerProvider()} and 
   * {@code GlobalTextMapPropagator::GetGlobalPropagator()}.
   *
   * @returns a {@code opentracing::Tracer}.
   */
  static inline TracerShim createTracerShim() 
  {
    return createTracerShim(opentelemetry::trace::Provider::GetTracerProvider());
  }

  /**
   * Creates a {@code opentracing::Tracer} shim using the provided 
   * {@code TracerProvider} and
   * {@code TextMapPropagator} instance.
   *
   * @param provider the {@code TracerProvider} instance used to create this shim.
   * @param propagators the {@code OpenTracingPropagators} instance used to create this shim.
   * @returns a {@code opentracing::Tracer}.
   */
  static inline TracerShim createTracerShim(const TracerProviderPtr& provider, 
                                            const OpenTracingPropagators& propagators = {}) 
  {
    return createTracerShim(provider->GetTracer("opentracing-shim"), propagators);
  }

  /**
   * Creates a {@code opentracing::Tracer} shim using provided
   * {@code Tracer} instance and
   * {@code GlobalTextMapPropagator::GetGlobalPropagator()}.
   *
   * @returns a {@code opentracing::Tracer}.
   */
  static inline TracerShim createTracerShim(const TracerPtr& tracer, 
                                            const OpenTracingPropagators& propagators = {}) 
  {
    return TracerShim(tracer, propagators);
  }

  std::unique_ptr<opentracing::Span> StartSpanWithOptions(opentracing::string_view operation_name, 
                                                          const opentracing::StartSpanOptions& options) const noexcept override;
  opentracing::expected<void> Inject(const opentracing::SpanContext& sc,
                                     std::ostream& writer) const override;
  opentracing::expected<void> Inject(const opentracing::SpanContext& sc,
                                     const opentracing::TextMapWriter& writer) const override;
  opentracing::expected<void> Inject(const opentracing::SpanContext& sc,
                                     const opentracing::HTTPHeadersWriter& writer) const override;
  opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(std::istream& reader) const override;
  opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::TextMapReader& reader) const override;
  opentracing::expected<std::unique_ptr<opentracing::SpanContext>> Extract(const opentracing::HTTPHeadersReader& reader) const override;
  inline void Close() noexcept override { is_closed_ = true; };

private:

  explicit TracerShim(const TracerPtr& tracer, const OpenTracingPropagators& propagators)
    : tracer_(tracer), propagators_(propagators) {}

  template <typename T>
  opentracing::expected<void> injectImpl(const opentracing::SpanContext& sc, 
                                         const T& writer, 
                                         const PropagatorPtr& propagator) const;

  template <typename T>
  opentracing::expected<std::unique_ptr<opentracing::SpanContext>> extractImpl(const T& reader, 
                                                                               const PropagatorPtr& propagator) const;
  
  TracerPtr tracer_;
  OpenTracingPropagators propagators_;
  bool is_closed_ = false;
};

} // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE