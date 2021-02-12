#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span,
                                  const opentelemetry::trace::SpanContext &parent_context) noexcept
{
  std::lock_guard<std::mutex> lock(mtx_);
  spans_.running.insert(static_cast<ThreadsafeSpanData *>(&span));
}

void TracezSpanProcessor::OnEnd(
    std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept
{
  if (span == nullptr)
    return;
  auto span_raw = static_cast<ThreadsafeSpanData *>(span.get());
  std::lock_guard<std::mutex> lock(mtx_);
  auto span_it = spans_.running.find(span_raw);
  if (span_it != spans_.running.end())
  {
    spans_.running.erase(span_it);
    spans_.completed.push_back(
        std::unique_ptr<ThreadsafeSpanData>(static_cast<ThreadsafeSpanData *>(span.release())));
  }
}

TracezSpanProcessor::CollectedSpans TracezSpanProcessor::GetSpanSnapshot() noexcept
{
  CollectedSpans snapshot;
  std::lock_guard<std::mutex> lock(mtx_);
  snapshot.running   = spans_.running;
  snapshot.completed = std::move(spans_.completed);
  spans_.completed.clear();
  return snapshot;
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
