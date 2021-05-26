// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

TracezDataAggregator::TracezDataAggregator(std::shared_ptr<TracezSharedData> shared_data,
                                           milliseconds update_interval)
{
  tracez_shared_data_ = shared_data;

  // Start a thread that calls AggregateSpans periodically or till notified.
  execute_.store(true, std::memory_order_release);
  aggregate_spans_thread_ = std::thread([this, update_interval]() {
    while (execute_.load(std::memory_order_acquire))
    {
      std::unique_lock<std::mutex> lock(mtx_);
      AggregateSpans();
      cv_.wait_for(lock, update_interval);
    }
  });
}

TracezDataAggregator::~TracezDataAggregator()
{
  // Notify and join the thread so object can be destroyed without wait for wake
  if (execute_.load(std::memory_order_acquire))
  {
    execute_.store(false, std::memory_order_release);
    cv_.notify_one();
    aggregate_spans_thread_.join();
  }
}

std::map<std::string, TracezData> TracezDataAggregator::GetAggregatedTracezData()
{
  std::unique_lock<std::mutex> lock(mtx_);
  return aggregated_tracez_data_;
}

LatencyBoundary TracezDataAggregator::FindLatencyBoundary(
    std::unique_ptr<ThreadsafeSpanData> &span_data)
{
  const auto &span_data_duration = span_data->GetDuration();
  for (unsigned int boundary = 0; boundary < kLatencyBoundaries.size() - 1; boundary++)
  {
    if (span_data_duration < kLatencyBoundaries[boundary + 1])
      return (LatencyBoundary)boundary;
  }
  return LatencyBoundary::k100SecondToMax;
}

void TracezDataAggregator::InsertIntoSampleSpanList(std::list<ThreadsafeSpanData> &sample_spans,
                                                    ThreadsafeSpanData &span_data)
{
  /**
   * Check to see if the sample span list size exceeds the set limit, if it does
   * free up memory and remove the earliest inserted sample before appending
   */
  if (sample_spans.size() == kMaxNumberOfSampleSpans)
  {
    sample_spans.pop_front();
  }
  sample_spans.push_back(ThreadsafeSpanData(span_data));
}

void TracezDataAggregator::ClearRunningSpanData()
{
  auto it = aggregated_tracez_data_.begin();
  while (it != aggregated_tracez_data_.end())
  {
    it->second.running_span_count = 0;
    it->second.sample_running_spans.clear();

    // Check if any data exists in the struct, if not delete entry
    bool is_completed_span_count_zero = true;
    for (const auto &completed_span_count : it->second.completed_span_count_per_latency_bucket)
    {
      if (completed_span_count > 0)
        is_completed_span_count_zero = false;
    }

    if (it->second.error_span_count == 0 && is_completed_span_count_zero)
    {
      it = aggregated_tracez_data_.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void TracezDataAggregator::AggregateStatusOKSpan(std::unique_ptr<ThreadsafeSpanData> &ok_span)
{
  // Find and update boundary of aggregated data that span belongs
  auto boundary_name = FindLatencyBoundary(ok_span);

  // Get the data for name in aggrgation and update count and sample spans
  auto &tracez_data = aggregated_tracez_data_.at(ok_span->GetName().data());
  InsertIntoSampleSpanList(tracez_data.sample_latency_spans[boundary_name], *ok_span.get());
  tracez_data.completed_span_count_per_latency_bucket[boundary_name]++;
}

void TracezDataAggregator::AggregateStatusErrorSpan(std::unique_ptr<ThreadsafeSpanData> &error_span)
{
  // Get data for name in aggregation and update count and sample spans
  auto &tracez_data = aggregated_tracez_data_.at(error_span->GetName().data());
  InsertIntoSampleSpanList(tracez_data.sample_error_spans, *error_span.get());
  tracez_data.error_span_count++;
}

void TracezDataAggregator::AggregateCompletedSpans(
    std::vector<std::unique_ptr<ThreadsafeSpanData>> &completed_spans)
{
  for (auto &completed_span : completed_spans)
  {
    std::string span_name = completed_span->GetName().data();

    if (aggregated_tracez_data_.find(span_name) == aggregated_tracez_data_.end())
    {
      aggregated_tracez_data_[span_name] = TracezData();
    }

    if (completed_span->GetStatus() == trace::StatusCode::kOk ||
        completed_span->GetStatus() == trace::StatusCode::kUnset)
      AggregateStatusOKSpan(completed_span);
    else
      AggregateStatusErrorSpan(completed_span);
  }
}

void TracezDataAggregator::AggregateRunningSpans(
    std::unordered_set<ThreadsafeSpanData *> &running_spans)
{
  for (auto &running_span : running_spans)
  {
    std::string span_name = running_span->GetName().data();

    if (aggregated_tracez_data_.find(span_name) == aggregated_tracez_data_.end())
    {
      aggregated_tracez_data_[span_name] = TracezData();
    }

    auto &tracez_data = aggregated_tracez_data_[span_name];
    InsertIntoSampleSpanList(aggregated_tracez_data_[span_name].sample_running_spans,
                             *running_span);
    tracez_data.running_span_count++;
  }
}

void TracezDataAggregator::AggregateSpans()
{
  auto span_snapshot = tracez_shared_data_->GetSpanSnapshot();
  /**
   * TODO: At this time in the project, there is no way of uniquely identifying
   * a span(their id's are not being set yet).
   * If in the future this is added then clearing of running spans will not bee
   * required.
   * For now this step of clearing and recalculating running span data is
   * required because it is unknown which spans have moved from running to
   * completed since the previous call. Additionally, the span name can change
   * for spans while they are running.
   *
   * A better approach for identifying moved spans would have been to map
   * span id to span name, find these span names in the aggregated data and then
   * delete only this information for running span data as opposed to clearing
   * all running span data. However this cannot be done at this time because,
   * unique identifiers to span data have not been added yet.
   *
   * A few things to note:
   * i) Duplicate running spans may be received from the span processor in one
   *    multiple successive calls to this function.
   * ii) Only the newly completed spans are received by this function.
   *     Completed spans will not be seen more than once
   **/
  ClearRunningSpanData();
  AggregateCompletedSpans(span_snapshot.completed);
  AggregateRunningSpans(span_snapshot.running);
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
