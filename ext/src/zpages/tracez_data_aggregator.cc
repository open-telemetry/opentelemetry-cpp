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

const std::map<std::string, std::unique_ptr<AggregatedInformation>>& TracezDataAggregator::GetAggregatedData()
{
  AggregateSpans();
  return aggregated_data_;
}

LatencyBoundaryName TracezDataAggregator::GetLatencyBoundary(opentelemetry::sdk::trace::Recordable* recordable)
{
  auto recordable_duration = recordable->GetDuration();
  for(unsigned int boundary = 0; boundary < kLatencyBoundaries.size()-1; boundary++)
  {
    if(recordable_duration < kLatencyBoundaries[boundary+1])return (LatencyBoundaryName)boundary;
  }
  return LatencyBoundaryName::k100SecondToMax;
}

void TracezDataAggregator::AggregateStatusOKSpan(std::unique_ptr<opentelemetry::sdk::trace::Recordable>& ok_span)
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

void TracezDataAggregator::AggregateStatusErrorSpan(std::unique_ptr<opentelemetry::sdk::trace::Recordable>& error_span)
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

void TracezDataAggregator::AggregateCompletedSpans()
{
  auto completed_spans = tracez_span_processor_->GetCompletedSpans();
  
  for(auto& span: completed_spans)
  {
    std::string span_name = span->GetName().data();
    
    if(aggregated_data_.find(span_name) == aggregated_data_.end())
    {
      aggregated_data_[span_name] = std::unique_ptr<AggregatedInformation>(new AggregatedInformation);
    }
    
    //running spans are calculated from scratch later
    aggregated_data_[span_name] -> num_running_spans = 0;
    
    if(span->GetStatus() == opentelemetry::trace::CanonicalCode::OK)AggregateStatusOKSpan(span);
    else AggregateStatusErrorSpan(span);
  }
}

void TracezDataAggregator::AggregateRunningSpans()
{
  auto running_spans = tracez_span_processor_->GetRunningSpans();
  std::unordered_set<std::string> cache;
  for(auto& span: running_spans)
  {
    std::string span_name = span->GetName().data();
 
    if(aggregated_data_.find(span_name) == aggregated_data_.end())
    {
      aggregated_data_[span_name] = std::unique_ptr<AggregatedInformation>(new AggregatedInformation);
    }
    
    if(cache.find(span_name) == cache.end())
    {
      aggregated_data_[span_name] -> num_running_spans = 0;
      cache.insert(span_name);
    }
    aggregated_data_[span_name] -> num_running_spans++;
  }
}


void TracezDataAggregator::AggregateSpans()
{
  AggregateCompletedSpans();
  AggregateRunningSpans();
}


}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
