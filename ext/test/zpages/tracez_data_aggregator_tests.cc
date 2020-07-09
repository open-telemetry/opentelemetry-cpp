#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::v0::trace::Span;
using opentelemetry::core::SteadyTimestamp;

const std::string span_name1 = "span 1"; 
const std::string span_name2 = "span 2";
const std::string span_name3 = "span 3";

/** Test to check if data aggrgator works as expected when there are no spans **/
TEST(TracezDataAggregator, NoSpans)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 0); 
}

/** Test to check if data aggrgator works as expected when there are is exactly one single running span **/
TEST(TracezDataAggregator, SingleRunningSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start the span get the data
  auto span_first  = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  //Check the size and to see if the running span count is accurate
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_running_spans, 1);
  
  //Make sure the rest of the data is not affected
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(aggregated_data->num_error_spans, 0);
  ASSERT_TRUE(aggregated_data->error_sample_spans.empty());
}

/** Test to check if data aggregator works as expected when there is exactly one completed span **/
TEST(TraceZDataAggregator, SingleCompletedSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start and end the span at a specified times
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));
  tracer->StartSpan(span_name1, start)->End(end);
  
  //Get the data and make sure span name exists in the data
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto& aggregated_data = data.at(span_name1);
  
  //Check if the span is correctly updated in the first boundary
  ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[LatencyBoundaryName::k0MicroTo10Micro], 1);
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].size(),1);
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].front()->GetDuration(),std::chrono::nanoseconds(30));
      
  //Make sure no other data is changed
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans,0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(),0);
  
  for(LatencyBoundaryName boundary = k10MicroTo100Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
  } 
}

/** Test to check if data aggregator works as expected when there is exactly one error span **/
TEST(TraceZDataAggregator, SingleErrorSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  // Start and end a single error span
  tracer->StartSpan(span_name1)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,"span cancelled");
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  //Check to see if span name can be found in aggregation
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto& aggregated_data = data.at(span_name1);
  
  //Check to see if error span that is introduced updates the required fields (number of error spans and error samples)
  ASSERT_EQ(aggregated_data -> num_error_spans, 1);
  ASSERT_EQ(aggregated_data -> error_sample_spans.size(),1);
  ASSERT_EQ(aggregated_data -> error_sample_spans.front()->GetName(), span_name1);
  
  //Make sure no other information is affected
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  
}

/** Test to check if multiple running spans behaves as expected**/ 
TEST(TracezDataAggregator, MultipleRunningSpans)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //A container that maps a span name to the number of spans to start with that span name
  std::unordered_map<std::string, int> running_span_name_to_count({
    {span_name1, 1},
    {span_name2, 2},
    {span_name3, 3},
  });
  // Start and store spans based on the above map
  std::vector<nostd::unique_ptr<Span>> running_span_container;
  for(auto span_name: running_span_name_to_count)
  {
    for(int count = 0; count < span_name.second; count++)running_span_container.push_back(tracer->StartSpan(span_name.first));
  }
  
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),3);
  
  //Check to see if the running span counts were updated correctly
  for(auto span_name: running_span_name_to_count)
  {
    ASSERT_TRUE(data.find(span_name.first) != data.end());
    ASSERT_EQ(data.at(span_name.first)->num_running_spans, span_name.second);
  }

  //Make sure rest of the data is unchanged
  for(auto& aggregated_data : data)
  {
    std::string span_name = aggregated_data.first;
    for(auto& latency_sample: data.at(span_name)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
    for(auto& completed_span_count: data.at(span_name)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
    ASSERT_EQ(data.at(span_name)->num_error_spans, 0);
    ASSERT_TRUE(data.at(span_name)->error_sample_spans.empty());
  }
}

/** Test to check if multiple completed spans updates the aggregated data correctly **/
TEST(TraceZDataAggregator, MultipleCompletedSpan)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  // Start spans with span name and the corresponding durations in one of the 9 latency buckets
  std::unordered_map<std::string, std::vector<std::vector<nanoseconds>>> span_name_to_duration({
    {span_name1, {{nanoseconds(0),nanoseconds(9999)},{},{},{},{},{},{},{},{}}},
    {span_name2, {{},{nanoseconds(10000), nanoseconds(99999)}, {nanoseconds(100000)}, {}, {}, {}, {}, {}, {}}},
    {span_name3, {{}, {}, {}, {nanoseconds(1000000), nanoseconds(9999999)}, {}, {}, {}, {}, {nanoseconds(9999999999999)}}}
  });
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  for(auto& span: span_name_to_duration)
  {
    for(auto& buckets: span.second)
    {
      for(auto& duration: buckets)
      {
        long long int end_time = duration.count()+1;
        start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(1));
        end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(end_time));
        tracer->StartSpan(span.first, start)->End(end);
      }
    }
  }
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),span_name_to_duration.size());
  
  for(auto& span: span_name_to_duration)
  {
    ASSERT_TRUE(data.find(span.first) != data.end());
    auto& aggregated_data = data.at(span.first);
    
    //Check if latency samples are in correct boundaries
    for(LatencyBoundaryName boundary = LatencyBoundaryName::k0MicroTo10Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
    {
        ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], span.second[boundary].size());
        ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),span.second[boundary].size());
        
        auto latency_sample = aggregated_data->latency_sample_spans[boundary].begin();
        for(unsigned int idx = 0 ; idx < span.second[boundary].size(); idx++)
        {
          ASSERT_EQ(span.second[boundary][idx],latency_sample->get()->GetDuration());
          latency_sample = std::next(latency_sample);
        }
    }
    
    //Check if nothing else is affected
    ASSERT_TRUE(aggregated_data->error_sample_spans.empty());
    ASSERT_EQ(aggregated_data->num_running_spans,0);
    ASSERT_EQ(aggregated_data->num_error_spans,0);
  }
}


/** 
 * This test checks to see if the aggregated data is updated correctly when there are multiple error spans.
 * It checks both the count of error spans and the error samples
 */ 
TEST(TraceZDataAggregator, MultipleErrorSpans)
{
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Container to store the span names --> error messges for the span name
  std::unordered_map<std::string, std::vector<std::string>> span_name_to_error({
    {span_name1, {"span 1 error"}},
    {span_name2, {"span 2 error 1", "span 2 error 2"}},
    {span_name3, {"span 3 error 1", "span 3 error 2", "span 3 error 3", "span 3 error 4", "span 3 error 5"}}
  });
  
  //Start spans with the error messages based on the map
  for(auto& span_error: span_name_to_error)
  {
    for(auto error_desc: span_error.second)tracer->StartSpan(span_error.first)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,error_desc);
  }

  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),span_name_to_error.size());
  
  //Check if error spans were updated correctly for the different span names
  for(auto& span_error: span_name_to_error)
  {
    // First try to find the span name in aggregation, then check the count of the error spans and then check values
    ASSERT_TRUE(data.find(span_error.first) != data.end());
    
    auto& aggregated_data = data.at(span_error.first);
    
    ASSERT_EQ(aggregated_data->num_error_spans, (int)span_error.second.size()); 
    ASSERT_EQ(aggregated_data->error_sample_spans.size(),span_error.second.size());
    
    auto error_sample = aggregated_data->error_sample_spans.begin();
    for(unsigned int idx = 0; idx < span_error.second.size(); idx++)
    {
      ASSERT_EQ(span_error.second[idx], error_sample->get()->GetDescription());
      error_sample = std::next(error_sample);
    }
    std::cout << "\n";
  }
  
  //Check if remaining information remains unaffected
  for(auto& aggregated_data : data)
  {
    std::string span_name = aggregated_data.first;
    ASSERT_EQ(data.at(span_name)->num_running_spans, 0);
    for(auto& latency_sample: data.at(span_name)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
    for(auto& completed_span_count: data.at(span_name)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  }
}

/** This test checks to see that there is no double counting of running spans when get aggregated data is called twice**/
TEST(TraceZDataAggregator, AdditionToRunningSpans)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Start a span and check the data
  auto span_first = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& temp_data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(temp_data.size(),1);
  ASSERT_TRUE(temp_data.find(span_name1) != temp_data.end());
  ASSERT_EQ(temp_data.at(span_name1)->num_running_spans, 1);
  
  //Start another span and check to see if there is no double counting of spans
  auto span_second = tracer->StartSpan(span_name1);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1)->num_running_spans, 2);
  
  //Make sure rest of the data is unchanged
  for(auto& latency_sample: data.at(span_name1)->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: data.at(span_name1)->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(data.at(span_name1)->num_error_spans, 0);
  ASSERT_TRUE(data.at(span_name1)->error_sample_spans.empty());
}

/** This test checks to see that once a running span is completed it the aggregated data is updated correctly **/
TEST(TraceZDataAggregator, RemovalOfRunningSpanWhenCompleted)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));

  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(std::chrono::nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(std::chrono::nanoseconds(40));
  
  //Start a span and make sure data is updated
  auto span_first = tracer->StartSpan(span_name1,start);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& temp_data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(temp_data.size(),1);
  ASSERT_TRUE(temp_data.find(span_name1) != temp_data.end());
  ASSERT_EQ(temp_data.at(span_name1)->num_running_spans, 1);
  
  //End the span and make sure running span is removed and completed span is updated, there should be only one completed span
  span_first->End(end);
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  //Check if completed span fields are correctly updated
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[LatencyBoundaryName::k0MicroTo10Micro], 1);
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].size(),1);
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].front()->GetDuration(),std::chrono::nanoseconds(30));
  
  //Check if unrelated data is unchanged
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans,0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(),0);
  
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k10MicroTo100Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
    ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
    ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
  }
}


/** 
 * This test checks to see that the maximum number of error samples(5) for a bucket is not exceeded.
 * If there are more spans than this for a single bucket it removes the earliest span that was recieved
 */
TEST(TraceZDataAggregator, ErrorSampleSpansOverCapacity)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  //Create error spans with the descriptions in the vector
  std::vector<std::string> span_error_descriptions = {"error span 1","error span 2","error span 3","error span 4","error span 5","error span 6"};
  for(auto span_error_description: span_error_descriptions) tracer->StartSpan(span_name1)->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED,span_error_description);
  
  //Fetch data and check if span name is spresent
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  
  // Check if error spans are updated according to spans started
  auto& aggregated_data = data.at(span_name1);
  ASSERT_EQ(aggregated_data->num_error_spans, (int)span_error_descriptions.size());
  ASSERT_EQ(aggregated_data->error_sample_spans.size(), kMaxNumberOfSampleSpans);

  // Check if the latest 5 error spans exist out of the total 6 that were introduced
  auto error_sample = aggregated_data->error_sample_spans.begin();
  for(unsigned int idx = 1; idx < span_error_descriptions.size(); idx++)
  {
    ASSERT_EQ(error_sample->get()->GetDescription(), span_error_descriptions[idx]);
    error_sample = std::next(error_sample);
  }
  
  //Make sure other data is unchaged
  for(auto& latency_sample: aggregated_data->latency_sample_spans)ASSERT_TRUE(latency_sample.empty());
  for(auto& completed_span_count: aggregated_data->span_count_per_latency_bucket)ASSERT_EQ(completed_span_count,0);
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
}

/** 
 * This test checks to see that the maximum number of latency samples(5) for a bucket is not exceeded.
 * If there are more spans than this for a single bucket it removes the earliest span that was recieved
 */
TEST(TraceZDataAggregator, CompletedSampleSpansOverCapacity)
{
  //Setup
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto tracez_data_aggregator (new TracezDataAggregator(processor));
  
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  
  //Start and end 6 spans with the same name that fall into the first latency bucket
  std::vector<std::pair<nanoseconds,nanoseconds>> timestamps = {
    make_pair(nanoseconds(10),nanoseconds(100)),
    make_pair(nanoseconds(1),nanoseconds(10000)),
    make_pair(nanoseconds(1000),nanoseconds(3000)),
    make_pair(nanoseconds(12),nanoseconds(12)),
    make_pair(nanoseconds(10),nanoseconds(5000)),
    make_pair(nanoseconds(10),nanoseconds(60))
  };
  for(auto timestamp: timestamps)
  {
    start.start_steady_time = SteadyTimestamp(timestamp.first);
    end.end_steady_time = SteadyTimestamp(timestamp.second);
    tracer->StartSpan(span_name1, start)->End(end);
  }
  
  //Get data and check if span name exists in aggregation
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& data = tracez_data_aggregator->GetAggregatedData();
  ASSERT_EQ(data.size(),1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  auto& aggregated_data = data.at(span_name1);
  
  //Check the count of completed spans in the buckets and the samples stored
  ASSERT_EQ(aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].size(), 5);
  ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[LatencyBoundaryName::k0MicroTo10Micro],timestamps.size());
  
  auto latency_sample = aggregated_data->latency_sample_spans[LatencyBoundaryName::k0MicroTo10Micro].begin();
  //idx starts from 1 and not 0 because there are 6 completed spans in the same bucket the and the first one is removed
  for(unsigned int idx = 1; idx < timestamps.size(); idx++)
  {
    ASSERT_EQ(latency_sample->get()->GetDuration().count(), timestamps[idx].second.count()-timestamps[idx].first.count());
    latency_sample = std::next(latency_sample);
  }
  
  //Make sure all other information is unchanged
  for(LatencyBoundaryName boundary = LatencyBoundaryName::k10MicroTo100Micro; boundary != LatencyBoundaryName::k100SecondToMax; ++boundary)
  {
      ASSERT_EQ(aggregated_data->span_count_per_latency_bucket[boundary], 0);
      ASSERT_EQ(aggregated_data->latency_sample_spans[boundary].size(),0);
  }
  ASSERT_EQ(aggregated_data->num_running_spans, 0);
  ASSERT_EQ(aggregated_data->num_error_spans, 0);
  ASSERT_EQ(aggregated_data->error_sample_spans.size(), 0);
}


