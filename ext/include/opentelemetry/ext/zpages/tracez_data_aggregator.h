#pragma once

// include libraries
#include <string>
#include <map>
#include <unordered_set>
#include <vector>
#include <list>

// include files
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/ext/zpages/latency_boundaries.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"


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
struct AggregatedInformation{
  
  int running_spans_; 
  int error_spans_;
  
  /** 
   * latency_sample_spans_ is a vector of lists, each index of the vector corresponds to a latency boundary(of which there are 9).
   * The list in each index stores the sample spans for that latency boundary.
   */
  std::vector<std::list<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>> latency_sample_spans_;
  
  /**
   * error_sample_spans_ is a list that stores the error saamples for a span name.
   */
  std::list<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> error_sample_spans_;
  
  /**
   * span_count_per_latency_bucket_ is a vector that stores the count of spans for each of the 9 latency buckets.
   */
  std::vector<int> span_count_per_latency_bucket_;
  
  AggregatedInformation()
  {
    running_spans_ = 0;
    error_spans_ = 0;
    latency_sample_spans_.resize(kNumberOfLatencyBoundaries);
    span_count_per_latency_bucket_.resize(kNumberOfLatencyBoundaries,0);
  } 
};

/**
 * This class is responsible for collecting raw data and converting it to useful information that can be made available to the user.
 */
class TracezDataAggregator
{
public:
  TracezDataAggregator(std::shared_ptr<TracezSpanProcessor> spanProcessor);
  
  /** 
   * AggregateSpans is the function that is called to update the aggregated data with newly
   * completed and running span information
   */
  void AggregateSpans();
  
  /** 
   * GetAggregatedData gets the aggregated span information
   * @returns a map with the span name as key and the aggregated information for the span name as value.
   */
  const std::map<std::string, std::unique_ptr<AggregatedInformation>>& GetAggregatedData();
  
  
private:
  /**Instance of the span processor to collect raw data**/
  std::shared_ptr<TracezSpanProcessor> tracez_span_processor_;
  
  /**
   * Tree map with key being the name of the span and value being a vector of size 9(number of latency boundaries), 
   * with each index value representing number of spans with a particular name and latency boundary. 
   * A hash map is not used because the span names are displayed in sorted order
   */
  std::map<std::string, std::unique_ptr<AggregatedInformation>> aggregated_data_;
  
  /**
   * GetLatencyBoundary returns the latency boundary to which the latency of the given recordable belongs to
   * @ param recordable is the recordable for which the latency boundary is to be found
   * @ returns LatencyBoundaryName is the name of the latency boundary that the recordable belongs to
   */
  LatencyBoundaryName GetLatencyBoundary(opentelemetry::sdk::trace::Recordable* recordable);
  
  /** 
   * AggregateCompletedSpans is the function that is called to update the aggregated data of newly completed spans
   */
  void AggregateCompletedSpans();
  
  /**
   * AggregateRunningSpans aggregates the data for running spans.
   */
  void AggregateRunningSpans();
  
  
  /** 
   * AggregateStatusOKSpans is the function called to update the data of spans with status code OK.
   * @param ok_span is the span who's data is to be collected
   */
  void AggregateStatusOKSpans(std::unique_ptr<opentelemetry::sdk::trace::Recordable>& ok_span);
  
  /** 
   * AggregateStatusErrorSpans is the function that is called to collect the information of error spans
   * @param error_span is the error span who's data is to be collected
   */
  void AggregateStatusErrorSpans(std::unique_ptr<opentelemetry::sdk::trace::Recordable>& error_span);
};

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
