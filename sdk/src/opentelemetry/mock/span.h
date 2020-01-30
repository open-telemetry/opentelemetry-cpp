#pragma once

#include "sdk/src/opentelemetry/mock/tracer.h"

namespace opentelemetry {
namespace sdk {
namespace mock {
class Span final : public trace::Span {
 public:
   explicit Span(std::shared_ptr<Tracer>&& tracer) noexcept;

   // trace::Tracer
   trace::Tracer &tracer() const noexcept override { return *tracer_; }

 private:
   std::shared_ptr<Tracer> tracer_;
};
}  // namespace mock
} // namespace sdk
} // namespace opentelemetry
