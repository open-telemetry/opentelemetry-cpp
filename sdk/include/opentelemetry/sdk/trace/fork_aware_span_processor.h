#pragma once

#include <mutex>
#include <vector>

#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * A Span Processor with added handlers that get called on fork events.
 *
 * Note: This is meant to support forking from a single thread that owns a
 * span processor. If multiple threads are creating span processors or interacting with a
 * single tracer while one of the threads forks, the process may well end up in
 * an invalid state because mutexes might be left locked.
 */
class ForkAwareSpanProcessor : public SpanProcessor
{
public:
  ForkAwareSpanProcessor() noexcept;

  ~ForkAwareSpanProcessor() noexcept override;

  /**
   * Handler called prior to forking.
   */
  virtual void PrepareForFork() noexcept {}

  /**
   * Handler called in the parent process after a fork.
   */
  virtual void OnForkedParent() noexcept {}

  /**
   * Handler called in the child process after a fork.
   */
  virtual void OnForkedChild() noexcept {}

  /**
   * @return a vector of the active span processors.
   */
  static std::vector<const ForkAwareSpanProcessor *> GetActiveSpanProcessorsForTesting();

private:
  static std::mutex mutex_;
  static ForkAwareSpanProcessor *active_span_processors_;

  // Note: We use an intrusive linked list so as to avoid any lifetime issues
  // associated with global variables with destructors.
  ForkAwareSpanProcessor *next_span_processor_{nullptr};

  static void PrepareForkHandler() noexcept;

  static void ParentForkHandler() noexcept;

  static void ChildForkHandler() noexcept;

  static void SetupForkHandlers() noexcept;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE