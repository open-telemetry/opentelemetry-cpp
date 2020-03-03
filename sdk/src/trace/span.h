#pragma once

#include "sdk/src/trace/tracer.h"

#include <mutex>

namespace opentelemetry {
namespace sdk {
namespace trace {
namespace trace_api = opentelemetry::trace;

class Span final : public trace_api::Span {
 public:
   explicit Span(std::shared_ptr<Tracer> &&tracer,
                 nostd::string_view name,
                 const trace_api::StartSpanOptions &options) noexcept;

 private:
   std::shared_ptr<Tracer> tracer_;
   std::mutex mutex_;
   std::unique_ptr<Recordable> recordable_;
};
} // namespace trace
} // namespace sdk
} // namespace opentelemetry
