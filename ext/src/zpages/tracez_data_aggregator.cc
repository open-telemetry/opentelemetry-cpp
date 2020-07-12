#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext{
namespace zpages{

TracezDataAggregator::TracezDataAggregator(
    std::shared_ptr<TracezSpanProcessor> span_processor){
  tracez_span_processor_ = span_processor;
}

const std::map<std::string, std::unique_ptr<TracezSpanData>>
    &TracezDataAggregator::GetAggregatedTracezData(){
  //Aggregate span data before returning the new information. 
  //The aggregation could also be periodically updated as an alternative
  
  std::lock_guard<std::mutex> lock_guard{mu_};
  AggregateSpans();
  return aggregated_tracez_data_;
}

LatencyBoundary TracezDataAggregator::FindLatencyBoundary(SpanData *span_data){
  auto span_data_duration = span_data->GetDuration();
  for (auto boundary = LatencyBoundary::k0MicroTo10Micro;
       boundary != LatencyBoundary::k100SecondToMax; ++boundary){
    if (span_data_duration < kLatencyBoundaries[boundary + 1])
      return boundary;
  }
  return LatencyBoundary::k100SecondToMax;
}

void TracezDataAggregator::InsertIntoSampleSpanList(
    std::list<std::unique_ptr<SpanData>>& sample_spans,
    std::unique_ptr<SpanData> &span_data){
  
  /** 
   * Check to see if the sample span list size exceeds the set limit, if it does
   * free up memory and remove the earliest inserted sample before appending
   */ 
  if (sample_spans.size() == kMaxNumberOfSampleSpans){
    sample_spans.front().reset();
    sample_spans.pop_front();
  }
  sample_spans.push_back(std::move(span_data));
}

void TracezDataAggregator::AggregateStatusOKSpan(
    std::unique_ptr<SpanData> &ok_span){
  //Find and update boundary of aggregated data that span belongs
  auto boundary_name = FindLatencyBoundary(ok_span.get());
  auto& tracez_data = aggregated_tracez_data_.at(ok_span->GetName().data());
  InsertIntoSampleSpanList(tracez_data->sample_latency_spans[boundary_name],ok_span);
  tracez_data->completed_span_count_per_latency_bucket[boundary_name]++;
}

void TracezDataAggregator::AggregateStatusErrorSpan(
    std::unique_ptr<SpanData> &error_span){
  auto& tracez_data = aggregated_tracez_data_.at(error_span->GetName().data());
  InsertIntoSampleSpanList(tracez_data->sample_error_spans,error_span);
  tracez_data->error_span_count++;
}

void TracezDataAggregator::AggregateCompletedSpans(
    std::vector<std::unique_ptr<SpanData>> &completed_spans){
  for (auto &span : completed_spans){
    std::string span_name = span->GetName().data();
    
    if (aggregated_tracez_data_.find(span_name) == aggregated_tracez_data_.end()){
      aggregated_tracez_data_[span_name] = std::unique_ptr<TracezSpanData>(new TracezSpanData);
    }
    
    // running spans are calculated from scratch set them to 0 now and recalculate them later
    aggregated_tracez_data_[span_name]->running_span_count = 0;
    aggregated_tracez_data_[span_name]->sample_running_spans.clear();

    if (span->GetStatus() == CanonicalCode::OK)
      AggregateStatusOKSpan(span);
    else
      AggregateStatusErrorSpan(span);
  }
}

void TracezDataAggregator::AggregateRunningSpans(
    std::unordered_set<SpanData *> &running_spans){
  std::unordered_set<std::string> seen_span_names;
  for (auto &span : running_spans){
    std::string span_name = span->GetName().data();

    if (aggregated_tracez_data_.find(span_name) == aggregated_tracez_data_.end()){
      aggregated_tracez_data_[span_name] = std::unique_ptr<TracezSpanData>(new TracezSpanData);
    }

    // If it's the first time this span name is seen, reset its information 
    // to avoid double counting from previous aggregated data. (described below)
    if (seen_span_names.find(span_name) == seen_span_names.end()){
      aggregated_tracez_data_[span_name]->running_span_count = 0;
      aggregated_tracez_data_[span_name]->sample_running_spans.clear();
      seen_span_names.insert(span_name);
    }

    // Maintain maximum size of sample running spans list
    if (aggregated_tracez_data_[span_name]->sample_running_spans.size() == kMaxNumberOfSampleSpans){
      aggregated_tracez_data_[span_name]->sample_running_spans.pop_front();
    }
    aggregated_tracez_data_[span_name]->sample_running_spans.push_back(span);
    aggregated_tracez_data_[span_name]->running_span_count++;
  }
}

void TracezDataAggregator::AggregateSpans(){
  auto span_snapshot = tracez_span_processor_->GetSpanSnapshot();
  /**
   * The following functions must be called in this particular order.
   * Calculation of running spans is stateless and does not rely on previous 
   * storage ie. everytime this function is called running span information
   * for every span name is assumed to be 0.
   *
   * All running span information present in the aggregated data can be set to 0
   * before these functions are called but that would take an additional linear step.
   *
   * To avoid paying this price, some work is done in aggergate completed spans 
   * to ensure that going into aggregate running spans all the span names
   * seen in completed spans have thier running span information reset.
   *
   * This is possible to do because if running spans with the same name exists it will be
   * aggregated in the AggregateRunningSpans function call that follows. Additionally if it's the
   * first time we are seeing a running span in AggregateRunningSpans, we set it to 0 to avoid double
   * counting the running span if it already existed in the aggregation from the previous call to
   * this function. See tests AdditionToRunningSpans and RemovalOfRunningSpanWhenCompleted to see an
   * example of where this is used.
   **/
  AggregateCompletedSpans(span_snapshot.completed);
  AggregateRunningSpans(span_snapshot.running);
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
