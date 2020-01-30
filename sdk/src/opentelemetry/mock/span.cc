#include "sdk/src/opentelemetry/mock/span.h"

namespace opentelemetry {
namespace sdk {
namespace mock {
Span::Span(std::shared_ptr<Tracer> &&tracer) noexcept : tracer_{std::move(tracer)} {}
}  // namespace mock
} // namespace sdk
} // namespace opentelemetry
