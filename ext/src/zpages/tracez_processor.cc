#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext {
namespace zpages {

  void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span) noexcept {
    if (shutdown_signal_received_) return;
    spans_.running.insert(&span);
  }

  void TracezSpanProcessor::OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept {
     if (shutdown_signal_received_ || span == nullptr) return;
     auto completed_span = spans_.running.find(span.get());
     if (completed_span != spans_.running.end()) {
       spans_.running.erase(completed_span);
       spans_.completed.push_back(std::move(span));
     }
  }


  TracezSpanProcessor::CollectedSpans TracezSpanProcessor::GetSpanSnapshot() noexcept {
    if (shutdown_signal_received_) return TracezSpanProcessor::CollectedSpans();
    CollectedSpans snapshot;
    snapshot.running = spans_.running;
    snapshot.completed = std::move(spans_.completed);
    spans_.completed.clear();
    return snapshot;
  }


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
