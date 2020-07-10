#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext {
namespace zpages {

  void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span) noexcept {
    if (shutdown_signal_received_) return;
    spans_.running.insert(static_cast<opentelemetry::sdk::trace::SpanData*>(&span));
  }

  void TracezSpanProcessor::OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept {
     if (shutdown_signal_received_ || span == nullptr) return;
     auto completed_span_raw = static_cast<opentelemetry::sdk::trace::SpanData*>(span.release());
     auto completed_span_it = spans_.running.find(completed_span_raw);
     if (completed_span_it != spans_.running.end()) {
       spans_.running.erase(completed_span_it);
       spans_.completed.push_back(
           std::unique_ptr<opentelemetry::sdk::trace::SpanData>(
               completed_span_raw));
     }
  }


  TracezSpanProcessor::CollectedSpans TracezSpanProcessor::GetSpanSnapshot() noexcept {
    CollectedSpans snapshot;
    if (!shutdown_signal_received_) {
      snapshot.running = spans_.running;
      snapshot.completed = std::move(spans_.completed);
      spans_.completed.clear();
    }
    return snapshot;
  }


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
