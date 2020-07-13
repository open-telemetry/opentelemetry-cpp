#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext {
namespace zpages {

  // TODO: Add thread safety measures
  void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span) noexcept {
    spans_.running.insert(static_cast<opentelemetry::sdk::trace::SpanData*>(&span));
  }

  // TODO: Add thread safety measures
  void TracezSpanProcessor::OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept {
     if (span == nullptr) return;
     auto span_raw = static_cast<opentelemetry::sdk::trace::SpanData*>(span.get());
     auto span_it = spans_.running.find(span_raw);
     if (span_it != spans_.running.end()) {
       spans_.running.erase(span_it);
       spans_.completed.push_back(
           std::unique_ptr<opentelemetry::sdk::trace::SpanData>(
               static_cast<opentelemetry::sdk::trace::SpanData*>(span.release())));
     }
  }


  // TODO: Add thread safety measures
  TracezSpanProcessor::CollectedSpans TracezSpanProcessor::GetSpanSnapshot() noexcept {
    CollectedSpans snapshot;
    snapshot.running = spans_.running;
    snapshot.completed = std::move(spans_.completed);
    spans_.completed.clear();
    return snapshot;
  }


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
