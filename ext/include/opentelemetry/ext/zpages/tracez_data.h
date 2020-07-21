#pragma once

#include <iostream>
#include <list>
#include <array>
#include <string>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"
#include "opentelemetry/ext/zpages/threadsafe_span_data.h"

using opentelemetry::ext::zpages::ThreadsafeSpanData;
using opentelemetry::trace::CanonicalCode;
using opentelemetry::trace::SpanId;
using opentelemetry::trace::TraceId;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext {
namespace zpages {

/** Convinient data structure for converting status code enum to string **/
const std::array<std::string,17> kStatusCodeToString = {
  "OK",
  "CANCELLED",
  "UNKNOWN",
  "INVALID_ARGUMENT",
  "DEADLINE_EXCEEDED",
  "NOT_FOUND",
  "ALREADY_EXISTS",
  "PERMISSION_DENIED",
  "RESOURCE_EXHAUSTED",
  "FAILED_PRECONDITION",
  "ABORTED",
  "OUT_OF_RANGE",
  "UNIMPLEMENTED",
  "INTERNAL",
  "UNAVAILABLE",
  "DATA_LOSS",
  "UNAUTHENTICATED",
};

/**
 * kMaxNumberOfSampleSpans is the maximum number of running, completed or error
 * sample spans stored at any given time for a given span name.
 * This limit is introduced to reduce memory usage by trimming sample spans
 * stored.
 */
const int kMaxNumberOfSampleSpans = 5;

/**
 * SampleSpanData is a structure that stores some of the relevant span
 * information for sample spans. The structure copies over data from ThreadsafeSpanData
 * and stores it as strings so it can readily be rendered on the frontend or
 * converted to JSON.
 * Defining this structure also eliminates the dependency on ThreadsafeSpanData of
 * running spans which is owned by span and access to which may cause race
 * conditions.
 * TODO: At this time ThreadsafeSpanData that is passed into this struct's constructor
 * does not have many attributes like span_id etc, so a lot of field's in the
 * struct will be empty.
 */
struct SampleSpanData {
  std::string span_name;
  std::string span_id;
  std::string trace_id;
  std::string parent_id;
  std::string description;
  long int duration;
  long int start_time;
  std::string status_code;
  SampleSpanData(ThreadsafeSpanData &span_data) {
    span_name = span_data.GetName().data();
    span_id = std::string(
        reinterpret_cast<const char *>(span_data.GetSpanId().Id().data()));
    trace_id = std::string(
        reinterpret_cast<const char *>(span_data.GetTraceId().Id().data()));
    parent_id = std::string(reinterpret_cast<const char *>(
        span_data.GetParentSpanId().Id().data()));
    description = span_data.GetDescription().data();
    duration = span_data.GetDuration().count();
    start_time = span_data.GetStartTime().time_since_epoch().count();
    status_code = kStatusCodeToString[(int)span_data.GetStatus()];
  }
};

/**
 * TracezData is the data to be displayed for tracez zpages that is stored for
 * each span name.
 */
struct TracezData {
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
  std::array<unsigned int, kLatencyBoundaries.size()>
      completed_span_count_per_latency_bucket;

  /**
   * sample_latency_spans is an array of lists, each index of the array
   * corresponds to a latency boundary(of which there are 9).
   * The list in each index stores the sample spans for that latency boundary.
   */
  std::array<std::list<SampleSpanData>, kLatencyBoundaries.size()>
      sample_latency_spans;

  /**
   * sample_error_spans is a list that stores the error samples for a span name.
   */
  std::list<SampleSpanData> sample_error_spans;

  /**
   * sample_running_spans is a list that stores the running span samples for a
   * span name.
   */
  std::list<SampleSpanData> sample_running_spans;

  TracezData() {
    running_span_count = 0;
    error_span_count = 0;
    completed_span_count_per_latency_bucket.fill(0);
  }
};

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
