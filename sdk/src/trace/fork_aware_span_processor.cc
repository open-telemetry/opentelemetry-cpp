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

#include "opentelemetry/sdk/trace/fork_aware_span_processor.h"
#include "src/common/platform/fork.h"

#include <cassert>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
std::mutex ForkAwareSpanProcessor::mutex_;

ForkAwareSpanProcessor *ForkAwareSpanProcessor::active_span_processors_{nullptr};

ForkAwareSpanProcessor::ForkAwareSpanProcessor() noexcept
{
  SetupForkHandlers();
  std::lock_guard<std::mutex> lock_guard{mutex_};
  next_span_processor_    = active_span_processors_;
  active_span_processors_ = this;
}

ForkAwareSpanProcessor::~ForkAwareSpanProcessor() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  if (this == active_span_processors_)
  {
    active_span_processors_ = active_span_processors_->next_span_processor_;
    return;
  }
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    if (span_processor->next_span_processor_ == this)
    {
      span_processor->next_span_processor_ = this->next_span_processor_;
      return;
    }
  }
  assert(0 && "ForkAwareSpanProcessor not found in global list");
}

std::vector<const ForkAwareSpanProcessor *>
ForkAwareSpanProcessor::GetActiveSpanProcessorsForTesting()
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  std::vector<const ForkAwareSpanProcessor *> result;
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    result.push_back(span_processor);
  }
  return result;
}

void ForkAwareSpanProcessor::PrepareForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->PrepareForFork();
  }
}

void ForkAwareSpanProcessor::ParentForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->OnForkedParent();
  }
}

void ForkAwareSpanProcessor::ChildForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->OnForkedChild();
  }
}

void ForkAwareSpanProcessor::SetupForkHandlers() noexcept
{
  static bool once = [] {
    common::platform::AtFork(PrepareForkHandler, ParentForkHandler, ChildForkHandler);
    return true;
  }();
  (void)once;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE