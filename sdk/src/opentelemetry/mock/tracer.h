#pragma once

#include <memory>

#include "opentelemetry/trace/tracer.h"

namespace opentelemetry {
namespace sdk {
namespace mock {
struct TracerConfig {
};

class Tracer final : public trace::Tracer, public std::enable_shared_from_this<Tracer> {
 public:
   explicit Tracer(TracerConfig&& config) noexcept;

   // trace::Tracer
   nostd::unique_ptr<trace::Span> StartSpan(
       nostd::string_view name,
       const trace::StartSpanOptions &options) noexcept override;

 private:
};
}  // namespace mock
} // namespace sdk
} // namespace opentelemetry
