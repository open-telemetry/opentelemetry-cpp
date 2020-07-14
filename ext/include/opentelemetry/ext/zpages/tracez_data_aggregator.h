#pragma once

#include <string>
#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <list>
#include <mutex>
#include <atomic>
#include <iostream>
#include <condition_variable>

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
 * This limit is introduced to reduce memory usage by trimming sample spans stored.
 */
const int kMaxNumberOfSampleSpans = 5;

/**
 * TracezSpanData is the span data to be displayed for tracez zpages that is 
 * stored for each span name. 
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
 * TracezDataAggregator object is responsible for collecting raw data and 
 * converting it to useful information that can be made available to 
 * display on the tracez zpage.
 *
 * NOTE: This class is not thread safe and is only expected to be called by a
 * single thread from the HTTP server
 */
class TracezDataAggregator{
public:
  /** 
   * Constructor runs a thread that calls a function to aggregate span data
   * at regular intervals.
   * @param span_processor is the tracez span processor to be set
   * @param update_interval_in_milliseconds the time duration for updating the 
   * aggregated data.
   */
  TracezDataAggregator(std::shared_ptr<TracezSpanProcessor> span_processor,
     long update_interval_in_milliseconds = 100);
  
  /** Ends the thread set up in the constructor and destroys the object **/
  ~TracezDataAggregator();
  
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
   * aggregation with the data of newly completed spans.
   * @param completed_spans are the newly completed spans.
   */
  void AggregateCompletedSpans(std::vector<std::unique_ptr<SpanData>>& 
  completed_spans);
  
  /**
   * AggregateRunningSpans aggregates the data for all running spans recieved
   * from the span processor. Running spans are not cleared by the span processor
   * and multiple calls to this function may contain running spans for which data
   * has already been collected in a previous call. 
   * There seems to be no trivial to way to know if it is a new or old running
   * span so at every call to this function the available running span data is 
   * reset and recalcuated.
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
   * @ returns LatencyBoundary is the latency boundary that the duration belongs to
   */
  LatencyBoundary FindLatencyBoundary(SpanData* span_data);
  
  /**
   * InsertIntoSampleSpanList is a helper function that is called to insert 
   * a given span into a sample span list. A function is used for insertion
   * because list size is to be limited at a set maximum.
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
   * A tree map is preferred to a hash map because the the data is to be ordered
   * in alphabetical order of span name.
   * TODO : A possible memory concern if there are too many unique 
   * span names, one solution could be to implement a LRU cache that trims the 
   * DS based on frequency of usage of a span name.
   */
  std::map<std::string, std::unique_ptr<TracezSpanData>> aggregated_tracez_data_;
  
  /** A boolean that is set to true in the constructor and false in the 
   * destructor to start and end execution of aggregate spans **/ 
  std::atomic<bool> execute_;
  /** Thread that executes aggregate spans at regurlar intervals during this 
  objects lifetime**/
  std::thread aggregate_spans_thread_;
  /** Condition variable that notifies the thread when object is about to be
  destroyed **/
  std::condition_variable cv_;
  std::mutex mtx_;
  
};

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
