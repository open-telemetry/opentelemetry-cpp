#pragma once

#include <mutex>
#include <vector>

#include "opentelemetry/sdk/trace/multi_recordable.h"
#include "opentelemetry/sdk/trace/processor.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/** Instantiation options. */
struct MultiSpanProcessorOptions
{};

/**
 * Span processor allow hooks for span start and end method invocations.
 *
 * Built-in span processors are responsible for batching and conversion of
 * spans to exportable representation and passing batches to exporters.
 */
class MultiSpanProcessor : public SpanProcessor
{
public:
  MultiSpanProcessor(std::vector<std::unique_ptr<SpanProcessor>> &&processors)
  {
    for (auto &processor : processors)
    {
      std::cout << " \nAdding processor...";
      if (processor)
        processors_.push_back(std::move(processor));
    }
  }

  void AddProcessor(std::unique_ptr<SpanProcessor> &&processor)
  {
    processors_.push_back(std::move(processor));
  }

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    std::cout << " Make recordable";
    auto recordable       = std::unique_ptr<Recordable>(new MultiRecordable);
    auto multi_recordable = static_cast<MultiRecordable *>(recordable.get());
    for (auto &processor : processors_)
    {
      multi_recordable->AddRecordable(*processor, processor->MakeRecordable());
    }
    return recordable;
  }

  virtual void OnStart(Recordable &span,
                       const opentelemetry::trace::SpanContext &parent_context) noexcept override
  {
    auto multi_recordable = static_cast<MultiRecordable *>(&span);

    for (auto &processor : processors_)
    {
      auto &recordable = multi_recordable->GetRecordable(*processor);
      if (recordable != nullptr)
      {
        processor->OnStart(*recordable, parent_context);
      }
    }
  }

  virtual void OnEnd(std::unique_ptr<Recordable> &&span) noexcept override
  {
    auto multi_recordable = static_cast<MultiRecordable *>(span.release());
    for (auto &processor : processors_)
    {
      auto recordable = multi_recordable->ReleaseRecordable(*processor);
      if (recordable != nullptr)
      {
        processor->OnEnd(std::move(recordable));
      }
    }
  }

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    bool result = true;
    for (auto &processor : processors_)
    {
      result |= processor->ForceFlush(timeout);
    }
    return result;
  }

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    bool result = true;
    for (auto &processor : processors_)
    {
      result |= processor->Shutdown(timeout);
    }
    return result;
  }

  ~MultiSpanProcessor() { Shutdown(); }

private:
  std::vector<std::unique_ptr<SpanProcessor>> processors_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE