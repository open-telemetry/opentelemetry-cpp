#include "tracer.h"

namespace {
class Span final : public opentelemetry::trace::Span {
 public:
   Span(std::shared_ptr<Tracer> &&tracer,
        opentelemetry::nostd::string_view name,
        const opentelemetry::trace::StartSpanOptions &options) noexcept
       : tracer_{std::move(tracer)}, name_{name}
   {}

   // opentelemetry::trace::Span
   Tracer& tracer() const noexcept override { return *tracer_; }

 private:
  std::shared_ptr<Tracer> tracer_;
  std::string name_;
};
} // namespace

Tracer::Tracer(opentelemetry::nostd::string_view output) {
  (void)output;
}

opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span> Tracer::StartSpan(
    opentelemetry::nostd::string_view name,
    const opentelemetry::trace::StartSpanOptions &options) noexcept try {
  return opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>{
      new (std::nothrow) Span{this->shared_from_this(), name, options}};
} catch(const std::exception& /*e*/) {
  return nullptr;
}
