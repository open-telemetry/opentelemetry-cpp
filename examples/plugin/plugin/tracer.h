#pragma once

#include <memory>

#include "opentelemetry/trace/tracer.h"

class Tracer final : public opentelemetry::trace::Tracer, 
  std::enable_shared_from_this<Tracer> {
 public:
   explicit Tracer(opentelemetry::nostd::string_view output);

   // opentelemetry::trace::Tracer
   opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span> StartSpan(
       opentelemetry::nostd::string_view name,
       const opentelemetry::trace::StartSpanOptions &options) noexcept override;

 private:

};
