#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

TracezDataAggregator::TracezDataAggregator(std::shared_ptr<TracezSpanProcessor> span_processor)
{
  tracez_span_processor_ = span_processor;
}

const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& TracezDataAggregator::GetAggregatedData()
{
  AggregateSpans();
  return aggregated_data_;
}

LatencyBoundaryName TracezDataAggregator::GetLatencyBoundary(SpanData* span_data)
{
  auto span_data_duration = span_data->GetDuration();
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k0MicroTo10Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
    if(span_data_duration < kLatencyBoundaries[boundary+1])return boundary;
  }
  return LatencyBoundaryName::k100SecondToMax;
}

void TracezDataAggregator::AggregateStatusOKSpan(std::unique_ptr<SpanData>& ok_span)
{
  LatencyBoundaryName boundary_name = GetLatencyBoundary(ok_span.get());
  std::string span_name = ok_span->GetName().data();
  
  //If the sample span storage is at capacity, remove the span that was inserted earliest and free up memory
  if(aggregated_data_[span_name]->latency_sample_spans[boundary_name].size() == kMaxNumberOfSampleSpans)
  {
    aggregated_data_[span_name]->latency_sample_spans[boundary_name].front().reset();
    aggregated_data_[span_name]->latency_sample_spans[boundary_name].pop_front();
  }
  aggregated_data_[span_name]->latency_sample_spans[boundary_name].push_back(std::move(ok_span));
  aggregated_data_[span_name]->span_count_per_latency_bucket[boundary_name]++;
}

void TracezDataAggregator::AggregateStatusErrorSpan(std::unique_ptr<SpanData>& error_span)
{
  std::string span_name = error_span->GetName().data();
  
  //If the error samples storage is at capacity, remove the span that was inserted earliest and free up memory
  if(aggregated_data_[span_name]->error_sample_spans.size() == kMaxNumberOfSampleSpans)
  {
    aggregated_data_[span_name]->error_sample_spans.front().reset();
    aggregated_data_[span_name]->error_sample_spans.pop_front();
  }
  aggregated_data_[span_name]->error_sample_spans.push_back(std::move(error_span));
  aggregated_data_[span_name]->num_error_spans++;
}

void TracezDataAggregator::AggregateCompletedSpans(std::vector<std::unique_ptr<SpanData>>& completed_spans)
{ 
  for(auto& span: completed_spans)
  {
    std::string span_name = span->GetName().data();
    
    if(aggregated_data_.find(span_name) == aggregated_data_.end())
    {
      aggregated_data_[span_name] = std::unique_ptr<AggregatedSpanData>(new AggregatedSpanData);
    }
    
    //running spans are calculated from scratch later
    aggregated_data_[span_name] -> num_running_spans = 0;
    aggregated_data_[span_name] -> running_sample_spans.clear();
    
    if(span->GetStatus() == CanonicalCode::OK)AggregateStatusOKSpan(span);
    else AggregateStatusErrorSpan(span);
  }
}

void TracezDataAggregator::AggregateRunningSpans(std::unordered_set<SpanData*>& running_spans)
{
  std::unordered_set<std::string> seen_span_names;
  for(auto& span: running_spans)
  {
    std::string span_name = span->GetName().data();
 
    if(aggregated_data_.find(span_name) == aggregated_data_.end())
    {
      aggregated_data_[span_name] = std::unique_ptr<AggregatedSpanData>(new AggregatedSpanData);
    }
    
    //If it's the first time this span name is seen, set the count to 0 to avoid double counting
    //from previous aggregated data.
    if(seen_span_names.find(span_name) == seen_span_names.end())
    {
      aggregated_data_[span_name] -> num_running_spans = 0;
      aggregated_data_[span_name] -> running_sample_spans.clear();
      seen_span_names.insert(span_name);
    }
    
    if(aggregated_data_[span_name]->running_sample_spans.size() == kMaxNumberOfSampleSpans)
    {
      aggregated_data_[span_name]->running_sample_spans.pop_front();
    }
    aggregated_data_[span_name]-> running_sample_spans.push_back(span);
    aggregated_data_[span_name] -> num_running_spans++;
  }
}


void TracezDataAggregator::AggregateSpans()
{
  auto span_snapshot = tracez_span_processor_->GetSpanSnapshot();
  /**
   * The following functions must be called in this particular order.
   * Running spans are calculated from scratch every time this function is called ie. assuming
   * all running spans are zero initially. 
   * All spans present in the aggregated data can be set to 0 before these functions
   * are called but that would take an additional linear step.
   * To avoid paying this price, some work is done in aggergate completed spans to ensure that 
   * going into aggregate running spans all the span names in completed spans have a running span value of 0.
   * This is possible to do because if running spans with the same name exists it will be aggregated in the 
   * AggregateRunningSpans function call that follows.
   * Additionally if it's the first time we are seeing a running span in AggregateRunningSpans, we set it to 0
   * to avoid double counting the running span if it already existed in the aggregation from the previous call to
   * this function. See tests AdditionToRunningSpans and RemovalOfRunningSpanWhenCompleted to see an example
   * of where this is used.
   **/
  AggregateCompletedSpans(span_snapshot.completed);
  AggregateRunningSpans(span_snapshot.running);
}


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
