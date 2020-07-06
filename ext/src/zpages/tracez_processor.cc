#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext {
namespace zpages {

  void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span) noexcept {
    running_spans_.insert(&span);
  }

  void TracezSpanProcessor::OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept {
     nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> batch(&span, 1);
     if (exporter_->Export(batch) == opentelemetry::sdk::trace::ExportResult::kFailure) {
       std::cerr << "Error batching span\n";
     }

     auto completed_span = running_spans_.find(span.get());
     if (completed_span != running_spans_.end()) {
       running_spans_.erase(completed_span);
       completed_spans_.push_back(std::move(span));
     }
  }

  std::unordered_set<opentelemetry::sdk::trace::Recordable*>& TracezSpanProcessor::GetRunningSpans() noexcept {
    return running_spans_;
  }

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> TracezSpanProcessor::GetCompletedSpans() noexcept {
    auto newly_completed_spans = std::move(completed_spans_);
    completed_spans_.clear();
    return newly_completed_spans;
  }


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
