// The MIT License (MIT)

// Copyright (c) 2015-2016 LightStep

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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