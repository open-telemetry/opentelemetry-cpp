#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Span &span,
                                  const opentelemetry::trace::SpanContext &parent_context) noexcept
{
  shared_data_->OnStart(static_cast<ThreadsafeSpanData *>(span.GetRecordablePtr().get()));
}

void TracezSpanProcessor::OnEnd(
    opentelemetry::sdk::trace::Span &span) noexcept
{
  auto recordable = span.ConsumeRecordable();
  if (recordable == nullptr)
    return;
  shared_data_->OnEnd(std::unique_ptr<ThreadsafeSpanData>(static_cast<ThreadsafeSpanData *>(recordable.release())));
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
