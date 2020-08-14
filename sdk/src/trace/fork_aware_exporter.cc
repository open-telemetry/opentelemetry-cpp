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

#include "opentelemetry/sdk/trace/fork_aware_exporter.h"
#include "src/common/platform/fork.h"

#include <cassert>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
std::mutex ForkAwareSpanExporter::mutex_;

ForkAwareSpanExporter *ForkAwareSpanExporter::active_span_exporters_{nullptr};

ForkAwareSpanExporter::ForkAwareSpanExporter() noexcept
{
  SetupForkHandlers();
  std::lock_guard<std::mutex> lock_guard{mutex_};
  next_span_exporter_    = active_span_exporters_;
  active_span_exporters_ = this;
}

ForkAwareSpanExporter::~ForkAwareSpanExporter() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  if (this == active_span_exporters_)
  {
    active_span_exporters_ = active_span_exporters_->next_span_exporter_;
    return;
  }
  for (auto span_exporter = active_span_exporters_; span_exporter != nullptr;
       span_exporter      = span_exporter->next_span_exporter_)
  {
    if (span_exporter->next_span_exporter_ == this)
    {
      span_exporter->next_span_exporter_ = this->next_span_exporter_;
      return;
    }
  }
  assert(0 && "ForkAwareSpanExporter not found in global list");
}

std::vector<const ForkAwareSpanExporter *> ForkAwareSpanExporter::GetActiveSpanExportersForTesting()
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  std::vector<const ForkAwareSpanExporter *> result;
  for (auto span_exporter = active_span_exporters_; span_exporter != nullptr;
       span_exporter      = span_exporter->next_span_exporter_)
  {
    result.push_back(span_exporter);
  }
  return result;
}

void ForkAwareSpanExporter::PrepareForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_exporter = active_span_exporters_; span_exporter != nullptr;
       span_exporter      = span_exporter->next_span_exporter_)
  {
    span_exporter->PrepareForFork();
  }
}

void ForkAwareSpanExporter::ParentForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_exporter = active_span_exporters_; span_exporter != nullptr;
       span_exporter      = span_exporter->next_span_exporter_)
  {
    span_exporter->OnForkedParent();
  }
}

void ForkAwareSpanExporter::ChildForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_exporter = active_span_exporters_; span_exporter != nullptr;
       span_exporter      = span_exporter->next_span_exporter_)
  {
    span_exporter->OnForkedChild();
  }
}

void ForkAwareSpanExporter::SetupForkHandlers() noexcept
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