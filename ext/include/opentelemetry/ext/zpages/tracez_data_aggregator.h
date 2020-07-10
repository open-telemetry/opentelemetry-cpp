#pragma once

// include libraries
#include <string>
#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <list>

// include files
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/ext/zpages/latency_boundaries.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"

using opentelemetry::sdk::trace::SpanData;
using opentelemetry::trace::CanonicalCode;


OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

/**
 * kMaxNumberOfSampleSpans is the maximum number of error and latency sample spans stored at any given time.
 */
const int kMaxNumberOfSampleSpans = 5;

/**
 * AggregatedInformation is the aggregated span information that is stored for each span name. 
 */
struct AggregatedSpanData{
  
  int num_running_spans; 
  int num_error_spans;
  
  
  /** 
   * latency_sample_spans is an array of lists, each index of the array corresponds to a latency boundary(of which there are 9).
   * The list in each index stores the sample spans for that latency boundary.
   */
  std::array<std::list<std::unique_ptr<SpanData>>,kLatencyBoundaries.size()> latency_sample_spans;
  
  /**
   * error_sample_spans is a list that stores the error samples for a span name.
   */
  std::list<std::unique_ptr<SpanData>> error_sample_spans;
  
  /**
   * running_sample_spans is a list that stores the running span samples for a span name.
   */
  std::list<SpanData*> running_sample_spans;
  
  /**
   * span_count_per_latency_bucket is a array that stores the count of spans for each of the 9 latency buckets.
   */
  std::array<int,kLatencyBoundaries.size()>span_count_per_latency_bucket;
  
  AggregatedSpanData()
  {
    num_running_spans = 0;
    num_error_spans = 0;
    span_count_per_latency_bucket.fill(0);
  } 
};

/**
 * TracezDataAggregator class is responsible for collecting raw data and converting it to useful information that can be made available to 
 * display for the tracez zpages.
 */
class TracezDataAggregator
{
public:
  TracezDataAggregator(std::shared_ptr<TracezSpanProcessor> spanProcessor);
  ~TracezDataAggregator();

  /** 
   * GetAggregatedData gets the aggregated span information
   * @returns a map with the span name as key and the aggregated information for the span name as value.
   */
  const std::map<std::string, std::unique_ptr<AggregatedSpanData>>& GetAggregatedData();
  
  
private:
  std::shared_ptr<TracezSpanProcessor> tracez_span_processor_;
  
  /**
   * Tree map with key being the name of the span and value being an array of size 9(number of latency boundaries), 
   * with each index value representing number of spans with a particular name and latency boundary. 
   * A hash map is not used because the span names need to be displayed in sorted order
   */
  std::map<std::string, std::unique_ptr<AggregatedSpanData>> aggregated_data_;
  
  
  /** 
   * AggregateSpans is the function that is called to update the aggregated data with newly
   * completed and running span information
   */
  void AggregateSpans();
  
  /** 
   * AggregateCompletedSpans is the function that is called to update the aggregated data of newly completed spans
   * @param completed_spans is the newly completed spans.
   */
  void AggregateCompletedSpans(std::vector<std::unique_ptr<SpanData>>& completed_spans);
  
  /**
   * AggregateRunningSpans aggregates the data for running spans. This function is stateless, it calculates
   * running spans from the very beggining. A stateless approach is used because there is no straightforward 
   * way to tell if a span was completed since the last call to this function.
   * @param running_spans is the running spans to be aggregated.
   */
  void AggregateRunningSpans(std::unordered_set<SpanData*>& running_spans);
  
  /** 
   * AggregateStatusOKSpans is the function called to update the data of spans with status code OK.
   * @param ok_span is the span who's data is to be aggregated
   */
  void AggregateStatusOKSpan(std::unique_ptr<SpanData>& ok_span);
  
  /** 
   * AggregateStatusErrorSpans is the function that is called to collect the information of error spans
   * @param error_span is the error span who's data is to be aggregated
   */
  void AggregateStatusErrorSpan(std::unique_ptr<SpanData>& error_span);

  /**
   * GetLatencyBoundary returns the latency boundary to which the latency of the given SpanData belongs to
   * @ param SpanData is the SpanData for which the latency boundary is to be found
   * @ returns LatencyBoundaryName is the name of the latency boundary that the SpanData belongs to
   */
  LatencyBoundaryName GetLatencyBoundary(SpanData* SpanData);
  
};

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
