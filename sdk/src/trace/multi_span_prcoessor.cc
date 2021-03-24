#include "opentelemetry/sdk/trace/multi_span_processor.h"

#include "opentelemetry/sdk/trace/span.h"
#include "opentelemetry/sdk/trace/exportable_span.h"

#include <vector>
using opentelemetry::trace::SpanContext;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

MultiSpanProcessor::MultiSpanProcessor(std::vector<std::unique_ptr<SpanProcessor>> processors,
                     const MultiSpanProcessorOptions &options)
  : processors_(std::move(processors))
{}

MultiSpanProcessor::~MultiSpanProcessor() {}


void MultiSpanProcessor::RegisterRecordable(ExportableSpan& span) noexcept
{
    std::vector<std::unique_ptr<Recordable>> joined;
    for (auto& processor : processors_)
    {
      processor->RegisterRecordable(span);
    }
}

void MultiSpanProcessor::OnStart(ExportableSpan &span, const SpanContext &context) noexcept
{
  for (auto& processor : processors_)
  {
      // TODO - wrap span so it only has visibility into specific processor recordable.
      processor->OnStart(span, context);
  }
}

void MultiSpanProcessor::OnEnd(std::unique_ptr<ExportableSpan> &&span) noexcept
{
  for (auto& processor : processors_)
  {
      processor->OnEnd(span->ReleaseExportableSpanFor(*processor));
  }
}

bool MultiSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept {
  bool result = true;
  // TODO: Attempt to cleanup in parallel or keep track of running timoeut timer...
  for (auto& processor : processors_)
  {
      result |= processor->ForceFlush(timeout);
  }
  return result;
}

bool MultiSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept {
  bool result = true;
  // TODO: Attempt to cleanup in parallel or keep track of running timoeut timer...
  for (auto& processor : processors_)
  {
      result |= processor->Shutdown(timeout);
  }
  return result;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
