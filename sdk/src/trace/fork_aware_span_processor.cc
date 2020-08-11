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

//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
ForkAwareSpanProcessor::ForkAwareSpanProcessor() noexcept
{
  SetupForkHandlers();
  std::lock_guard<std::mutex> lock_guard{mutex_};
  next_span_processor_    = active_span_processors_;
  active_span_processors_ = this;
}

//--------------------------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
// GetActiveRecordersForTesting
//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
// PrepareForkHandler
//--------------------------------------------------------------------------------------------------
void ForkAwareSpanProcessor::PrepareForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->PrepareForFork();
  }
}

//--------------------------------------------------------------------------------------------------
// ParentForkHandler
//--------------------------------------------------------------------------------------------------
void ForkAwareSpanProcessor::ParentForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->OnForkedParent();
  }
}

//--------------------------------------------------------------------------------------------------
// ChildForkHandler
//--------------------------------------------------------------------------------------------------
void ForkAwareSpanProcessor::ChildForkHandler() noexcept
{
  std::lock_guard<std::mutex> lock_guard{mutex_};
  for (auto span_processor = active_span_processors_; span_processor != nullptr;
       span_processor      = span_processor->next_span_processor_)
  {
    span_processor->OnForkedChild();
  }
}

//--------------------------------------------------------------------------------------------------
// SetupForkHandlers
//--------------------------------------------------------------------------------------------------
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