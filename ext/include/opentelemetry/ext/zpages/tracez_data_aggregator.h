#pragma once


#include <string>
#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <list>
#include <cinttypes>


#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/ext/zpages/latency_boundaries.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"

using opentelemetry::sdk::trace::SpanData;
using opentelemetry::trace::CanonicalCode;


OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext{
namespace zpages{

/**
 * kMaxNumberOfSampleSpans is the maximum number of running, completed or error
 * sample spans stored at any given time for a given span name. 
 * This limit is defined to reduce memory usage
 */
const int kMaxNumberOfSampleSpans = 5;

/**
 * TracezSpanData is the aggregated span information that is stored for 
 * each span name. 
 */
struct TracezSpanData{
  
  /** 
   * TODO: At this time the maximum count is unknown but a larger data type 
   * might have to be used in the future to store these counts to avoid overflow
   */
  unsigned int running_span_count; 
  unsigned int error_span_count;
  
  /**
   * completed_span_count_per_latency_bucket is an array that stores the count 
   * of spans for each of the 9 latency buckets.
   */
  std::array<unsigned int,kLatencyBoundaries.size()>completed_span_count_per_latency_bucket;
  
  /** 
   * sample_latency_spans is an array of lists, each index of the array 
   * corresponds to a latency boundary(of which there are 9).
   * The list in each index stores the sample spans for that latency boundary.
   */
  std::array<std::list<std::unique_ptr<SpanData>>,kLatencyBoundaries.size()> sample_latency_spans;
  
  /**
   * sample_error_spans is a list that stores the error samples for a span name.
   */
  std::list<std::unique_ptr<SpanData>> sample_error_spans;
  
  /**
   * sample_running_spans is a list that stores the running span samples for a 
   * span name.
   */
  std::list<SpanData*> sample_running_spans;

  TracezSpanData(){
    running_span_count = 0;
    error_span_count = 0;
    completed_span_count_per_latency_bucket.fill(0);
  } 
};

/**
 * TracezDataAggregator class is responsible for collecting raw data and 
 * converting it to useful information that can be made available to 
 * display on the tracez zpage.
 */
class TracezDataAggregator{
public:
  TracezDataAggregator(std::shared_ptr<TracezSpanProcessor> spanProcessor);

  /** 
   * GetAggregatedTracezData aggregates data and returns the the updated data.
   * @returns a map with the span name as key and the tracez span data as value.
   */
  const std::map<std::string, std::unique_ptr<TracezSpanData>>& GetAggregatedTracezData();
  
  
private:

  /** 
   * AggregateSpans is the function that is called to update the aggregated data
   * with newly completed and running span data
   */
  void AggregateSpans();
  
  /** 
   * AggregateCompletedSpans is the function that is called to update the 
   * aggregated data of newly completed spans
   * @param completed_spans are the newly completed spans.
   */
  void AggregateCompletedSpans(std::vector<std::unique_ptr<SpanData>>& 
  completed_spans);
  
  /**
   * AggregateRunningSpans aggregates the data for running spans. This function 
   * is stateless, it calculates running spans from the very beggining. 
   * A stateless approach is used because there is no straightforward way to 
   * tell if or which span was completed since the last call to this function.
   * @param running_spans is the running spans to be aggregated.
   */
  void AggregateRunningSpans(std::unordered_set<SpanData*>& running_spans);
  
  /** 
   * AggregateStatusOKSpans is the function called to update the data of spans 
   * with status code OK.
   * @param ok_span is the span who's data is to be aggregated
   */
  void AggregateStatusOKSpan(std::unique_ptr<SpanData>& ok_span);
  
  /** 
   * AggregateStatusErrorSpans is the function that is called to update the 
   * data of error spans
   * @param error_span is the error span who's data is to be aggregated
   */
  void AggregateStatusErrorSpan(std::unique_ptr<SpanData>& error_span);

  /**
   * FindLatencyBoundary returns the latency boundary to which the duration of 
   * the given span_data belongs to
   * @ param span_data is the SpanData whose duration for which the latency boundary
   * is to be found
   * @ returns enum LatencyBoundary is the name of the latency boundary 
   * that the span_data belongs to
   */
  LatencyBoundary FindLatencyBoundary(SpanData* span_data);
  
  /**
   * InsertIntoSampleSpanList is a helper function that is called to insert 
   * a given span into a sample span list. A function is used for insertion
   * because before list size is to be maintained at a preset number.
   * @param sample_spans the sample span list into which span is to be inserted
   * @param span_data the span_data to be inserted into list
   */
  void InsertIntoSampleSpanList(
    std::list<std::unique_ptr<SpanData>>& sample_spans,
    std::unique_ptr<SpanData> &span_data);
    
  /** Instance of span processor used to collect raw data **/
  std::shared_ptr<TracezSpanProcessor> tracez_span_processor_;
  
  /**
   * Tree map with key being the name of the span and value being a unique ptr
   * that stores the tracez span data for the given span name
   * A tree map is preferred to a hash map because the the data structure is
   * to be ordered in alphabetical order of span name.
   * TODO : A possible memory concern if there are too many unique 
   * span names, one solution could be to implement a LRU cache that trims the 
   * DS based on frequency of usage of a span name.
   */
  std::map<std::string, std::unique_ptr<TracezSpanData>> aggregated_tracez_data_;
  
};

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
