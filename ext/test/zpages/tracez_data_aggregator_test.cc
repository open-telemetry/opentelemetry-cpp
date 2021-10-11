// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

#include <gtest/gtest.h>

#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::common::SteadyTimestamp;
using opentelemetry::trace::Span;

const std::string span_name1 = "span 1";
const std::string span_name2 = "span 2";
const std::string span_name3 = "span 3";

/**
 * TODO: Due to the absence of way to simulate the passing of time in the
 * testing framework, synthetic delays had to be added in the tests to get the
 * object in question to perform correctly. Later on if something like this is
 * added the tests should be modified accordingly so that there is no external
 * dependency.
 * Additionally later on it would be better check for the span id(when set)
 * rather than span name.
 */

/** Test fixture for setting up the data aggregator and tracer for each test **/
class TracezDataAggregatorTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    std::shared_ptr<TracezSharedData> shared_data(new TracezSharedData());
    auto resource = opentelemetry::sdk::resource::Resource::Create({});
    std::unique_ptr<SpanProcessor> processor(new TracezSpanProcessor(shared_data));
    std::vector<std::unique_ptr<SpanProcessor>> processors;
    processors.push_back(std::move(processor));

    auto context           = std::make_shared<TracerContext>(std::move(processors), resource);
    tracer                 = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(context));
    tracez_data_aggregator = std::unique_ptr<TracezDataAggregator>(
        new TracezDataAggregator(shared_data, milliseconds(10)));
  }

  std::unique_ptr<TracezDataAggregator> tracez_data_aggregator;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;
};

/**
 * Helper function to check if the counts of running, error and latency spans
 * match what is expected
 */
void VerifySpanCountsInTracezData(
    const std::string &span_name,
    const TracezData &aggregated_data,
    size_t running_span_count,
    size_t error_span_count,
    std::array<unsigned int, kLatencyBoundaries.size()> completed_span_count_per_latency_bucket)
{
  // Asserts are needed to check the size of the container because they may need
  // to be checked and if size checks fail it must be stopped
  EXPECT_EQ(aggregated_data.running_span_count, running_span_count)
      << " Count of running spans incorrect for " << span_name << "\n";

  EXPECT_EQ(aggregated_data.sample_running_spans.size(),
            std::min<size_t>(running_span_count, kMaxNumberOfSampleSpans))
      << " Size of sample running spans incorrect for " << span_name << "\n";

  EXPECT_EQ(aggregated_data.error_span_count, error_span_count)
      << " Count of error spans incorrect for " << span_name << "\n";

  EXPECT_EQ(aggregated_data.sample_error_spans.size(),
            std::min<size_t>(error_span_count, kMaxNumberOfSampleSpans))
      << " Count of running spans incorrect for " << span_name << "\n";

  for (unsigned int boundary = 0; boundary < kLatencyBoundaries.size(); boundary++)
  {
    EXPECT_EQ(aggregated_data.completed_span_count_per_latency_bucket[boundary],
              completed_span_count_per_latency_bucket[boundary])
        << " Count of completed spans in latency boundary " << boundary << " incorrect for "
        << span_name << "\n";
    EXPECT_EQ(aggregated_data.sample_latency_spans[boundary].size(),
              std::min<size_t>(completed_span_count_per_latency_bucket[boundary],
                               kMaxNumberOfSampleSpans))
        << " Count of sample completed spans in latency boundary " << boundary << " incorrect for "
        << span_name << "\n";
  }
}

/**************************** No Span Test ************************************/

/** Test to check if data aggregator works as expected when there are no spans
 * **/
TEST_F(TracezDataAggregatorTest, NoSpans)
{
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 0);
}

/*********************** Single span tests ************************************/

/** Test to check if data aggregator works as expected when there are
 * is exactly a single running span **/
TEST_F(TracezDataAggregatorTest, SingleRunningSpan)
{
  // Start the span get the data
  auto span_first = tracer->StartSpan(span_name1);
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();

  // Check to see if span name exists
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto &aggregated_data = data.at(span_name1);

  // Verify span counts then content of spans
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  ASSERT_EQ(aggregated_data.sample_running_spans.size(), 1);
  ASSERT_EQ(aggregated_data.sample_running_spans.front().GetName().data(), span_name1);
  span_first->End();
}

/** Test to check if data aggregator works as expected when there is exactly one
 * completed span **/
TEST_F(TracezDataAggregatorTest, SingleCompletedSpan)
{
  // Start and end the span at a specified times
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(nanoseconds(40));
  tracer->StartSpan(span_name1, start)->End(end);

  // Get the data and make sure span name exists in the data
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  auto &aggregated_data = data.at(span_name1);
  // Make sure counts of spans are in order
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, 0, {1, 0, 0, 0, 0, 0, 0, 0, 0});

  // Check if the span is correctly updated in the first boundary
  ASSERT_EQ(aggregated_data.sample_latency_spans[LatencyBoundary::k0MicroTo10Micro].size(), 1);
  ASSERT_EQ(aggregated_data.sample_latency_spans[LatencyBoundary::k0MicroTo10Micro]
                .front()
                .GetDuration()
                .count(),
            30);
}

/** Test to check if data aggregator works as expected when there is exactly
 * one error span **/
TEST_F(TracezDataAggregatorTest, SingleErrorSpan)
{
  // Start and end a single error span
  auto span = tracer->StartSpan(span_name1);
  span->SetStatus(opentelemetry::trace::StatusCode::kError, "span cancelled");
  span->End();
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();

  // Check to see if span name can be found in aggregation
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  auto &aggregated_data = data.at(span_name1);
  // Make sure counts of spans are in order
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, 1, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  // Check the value of the error span introduced
  ASSERT_EQ(aggregated_data.sample_error_spans.size(), 1);
  ASSERT_EQ(aggregated_data.sample_error_spans.front().GetName().data(), span_name1);
}

/************************* Multiple span tests ********************************/

/** Test to check if multiple running spans behaves as expected**/
TEST_F(TracezDataAggregatorTest, MultipleRunningSpans)
{
  // A container that maps a span name to the number of spans to start with that
  // span name
  std::unordered_map<std::string, int> running_span_name_to_count({
      {span_name1, 1},
      {span_name2, 2},
      {span_name3, 3},
  });

  // Start and store spans based on the above map
  std::vector<nostd::shared_ptr<Span>> running_span_container;
  for (auto span_name : running_span_name_to_count)
  {
    for (int count = 0; count < span_name.second; count++)
      running_span_container.push_back(tracer->StartSpan(span_name.first));
  }

  // give time for aggregation and then get data
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), running_span_name_to_count.size());

  // Check to see if the running span counts were updated correctly
  for (auto &span_name : running_span_name_to_count)
  {
    ASSERT_TRUE(data.find(span_name.first) != data.end());

    // Make sure counts of spans are in order
    VerifySpanCountsInTracezData(span_name.first, data.at(span_name.first), span_name.second, 0,
                                 {0, 0, 0, 0, 0, 0, 0, 0, 0});

    ASSERT_EQ(data.at(span_name.first).sample_running_spans.size(), span_name.second);
    for (auto &span_sample : data.at(span_name.first).sample_running_spans)
    {
      ASSERT_EQ(span_sample.GetName().data(), span_name.first);
    }
  }

  for (auto i = running_span_container.begin(); i != running_span_container.end(); i++)
    (*i)->End();
}

/** Test to check if multiple completed spans updates the aggregated data
 * correctly **/
TEST_F(TracezDataAggregatorTest, MultipleCompletedSpan)
{
  // Start spans with span name and the corresponding durations in one of the 9
  // latency buckets
  const std::unordered_map<std::string, std::vector<std::vector<nanoseconds>>>
      span_name_to_duration(
          {{span_name1, {{nanoseconds(10), nanoseconds(4600)}, {}, {}, {}, {}, {}, {}, {}, {}}},
           {span_name2,
            {{},
             {nanoseconds(38888), nanoseconds(98768)},
             {nanoseconds(983251)},
             {},
             {},
             {},
             {},
             {},
             {}}},
           {span_name3,
            {{},
             {},
             {},
             {nanoseconds(1234567), nanoseconds(1234567)},
             {},
             {},
             {},
             {},
             {nanoseconds(9999999999999)}}}});
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  for (auto &span : span_name_to_duration)
  {
    for (auto &buckets : span.second)
    {
      for (auto &duration : buckets)
      {
        long long int end_time  = duration.count() + 1;
        start.start_steady_time = SteadyTimestamp(nanoseconds(1));
        end.end_steady_time     = SteadyTimestamp(nanoseconds(end_time));
        tracer->StartSpan(span.first, start)->End(end);
      }
    }
  }

  // Give time for aggregation and get data
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();

  ASSERT_EQ(data.size(), span_name_to_duration.size());

  for (auto &span : span_name_to_duration)
  {
    ASSERT_TRUE(data.find(span.first) != data.end());
    auto &aggregated_data = data.at(span.first);

    // Make sure counts of spans are in order
    VerifySpanCountsInTracezData(
        span.first, aggregated_data, 0, 0,
        {(unsigned int)span.second[0].size(), (unsigned int)span.second[1].size(),
         (unsigned int)span.second[2].size(), (unsigned int)span.second[3].size(),
         (unsigned int)span.second[4].size(), (unsigned int)span.second[5].size(),
         (unsigned int)span.second[6].size(), (unsigned int)span.second[7].size(),
         (unsigned int)span.second[8].size()});

    for (unsigned int boundary = 0; boundary < kLatencyBoundaries.size(); boundary++)
    {
      ASSERT_EQ(aggregated_data.sample_latency_spans[boundary].size(),
                span.second[boundary].size());
      auto latency_sample = aggregated_data.sample_latency_spans[boundary].begin();
      for (unsigned int idx = 0; idx < span.second[boundary].size(); idx++)
      {
        ASSERT_EQ(span.second[boundary][idx].count(), latency_sample->GetDuration().count());
        latency_sample = std::next(latency_sample);
      }
    }
  }
}

/**
 * This test checks to see if the aggregated data is updated correctly
 * when there are multiple error spans.
 * It checks both the count of error spans and the error samples
 */
TEST_F(TracezDataAggregatorTest, MultipleErrorSpans)
{
  // Container to store the span names --> error messges for the span name
  std::unordered_map<std::string, std::vector<std::string>> span_name_to_error(
      {{span_name1, {"span 1 error"}},
       {span_name2, {"span 2 error 1", "span 2 error 2"}},
       {span_name3,
        {"span 3 error 1", "span 3 error 2", "span 3 error 3", "span 3 error 4",
         "span 3 error 5"}}});

  // Start spans with the error messages based on the map
  for (auto &span_error : span_name_to_error)
  {
    for (auto error_desc : span_error.second)
    {
      auto span = tracer->StartSpan(span_error.first);
      span->SetStatus(opentelemetry::trace::StatusCode::kError, error_desc);
      span->End();
    }
  }

  // Give some time and then get data
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), span_name_to_error.size());

  // Check if error spans were updated correctly for the different span names
  for (auto &span_error : span_name_to_error)
  {
    // First try to find the span name in aggregation, then check the count of
    // the error spans and then check values
    ASSERT_TRUE(data.find(span_error.first) != data.end());

    auto &aggregated_data = data.at(span_error.first);

    // Make sure counts of spans are in order
    VerifySpanCountsInTracezData(span_error.first, aggregated_data, 0, span_error.second.size(),
                                 {0, 0, 0, 0, 0, 0, 0, 0, 0});
    ASSERT_EQ(aggregated_data.error_span_count, span_error.second.size());

    auto error_sample = aggregated_data.sample_error_spans.begin();
    for (unsigned int idx = 0; idx < span_error.second.size(); idx++)
    {
      ASSERT_EQ(span_error.second[idx], error_sample->GetDescription());
      error_sample = std::next(error_sample);
    }
  }
}

/************************ Sample spans tests **********************************/

/**
 * This test checks to see that the maximum number of running samples(5) for a
 * bucket is not exceeded. If there are more spans than this for a single bucket
 * it removes the earliest span that was received
 */
TEST_F(TracezDataAggregatorTest, RunningSampleSpansOverCapacity)
{
  int running_span_count = 6;
  // Start and store spans based on the above map
  std::vector<nostd::shared_ptr<Span>> running_span_container;
  for (int count = 0; count < running_span_count; count++)
    running_span_container.push_back(tracer->StartSpan(span_name1));

  std::this_thread::sleep_for(milliseconds(500));
  // Fetch data and check if span name is spresent
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  // Check if error spans are updated according to spans started
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 6, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  ASSERT_EQ(aggregated_data.sample_running_spans.size(), kMaxNumberOfSampleSpans);

  for (auto i = running_span_container.begin(); i != running_span_container.end(); i++)
  {
    (*i)->End();
  }
}

/**
 * This test checks to see that the maximum number of error samples(5) for a
 * bucket is not exceeded. If there are more spans than this for a single bucket
 * it removes the earliest span that was received
 */
TEST_F(TracezDataAggregatorTest, ErrorSampleSpansOverCapacity)
{
  // Create error spans with the descriptions in the vector
  std::vector<std::string> span_error_descriptions = {"error span 1", "error span 2",
                                                      "error span 3", "error span 4",
                                                      "error span 5", "error span 6"};
  for (auto span_error_description : span_error_descriptions)
  {
    auto span = tracer->StartSpan(span_name1);
    span->SetStatus(opentelemetry::trace::StatusCode::kError, span_error_description);
    span->End();
  }

  std::this_thread::sleep_for(milliseconds(500));

  // Fetch data and check if span name is spresent
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  std::this_thread::sleep_for(milliseconds(500));

  // Check if error spans are updated according to spans started
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, span_error_descriptions.size(),
                               {0, 0, 0, 0, 0, 0, 0, 0, 0});

  // Check if the latest 5 error spans exist out of the total 6 that were
  // introduced
  auto error_sample = aggregated_data.sample_error_spans.begin();
  for (unsigned int idx = 1; idx < span_error_descriptions.size(); idx++)
  {
    ASSERT_EQ(error_sample->GetDescription(), span_error_descriptions[idx]);
    error_sample = std::next(error_sample);
  }
}

/**
 * This test checks to see that the maximum number of latency samples(5) for a
 * bucket is not exceeded. If there are more spans than this for a single bucket
 * it removes the earliest span that was received
 */
TEST_F(TracezDataAggregatorTest, CompletedSampleSpansOverCapacity)
{
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;

  // Start and end 6 spans with the same name that fall into the first latency
  // bucket
  std::vector<std::pair<nanoseconds, nanoseconds>> timestamps = {
      make_pair(nanoseconds(10), nanoseconds(100)),
      make_pair(nanoseconds(1), nanoseconds(10000)),
      make_pair(nanoseconds(1000), nanoseconds(3000)),
      make_pair(nanoseconds(12), nanoseconds(12)),
      make_pair(nanoseconds(10), nanoseconds(5000)),
      make_pair(nanoseconds(10), nanoseconds(60))};
  for (auto timestamp : timestamps)
  {
    start.start_steady_time = SteadyTimestamp(timestamp.first);
    end.end_steady_time     = SteadyTimestamp(timestamp.second);
    tracer->StartSpan(span_name1, start)->End(end);
  }

  // Give some time and get data
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();

  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  std::this_thread::sleep_for(milliseconds(500));
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, 0,
                               {(unsigned int)timestamps.size(), 0, 0, 0, 0, 0, 0, 0, 0});

  // Check the count of completed spans in the buckets and the samples stored
  auto latency_sample =
      aggregated_data.sample_latency_spans[LatencyBoundary::k0MicroTo10Micro].begin();

  // idx starts from 1 and not 0 because there are 6 completed spans in the same
  // bucket the and the first one is removed
  for (unsigned int idx = 1; idx < timestamps.size(); idx++)
  {
    ASSERT_EQ(latency_sample->GetDuration().count(),
              timestamps[idx].second.count() - timestamps[idx].first.count());
    latency_sample = std::next(latency_sample);
  }
}

/************************* Miscellaneous tests ********************************/

/** Test to see if the span names are in alphabetical order **/
TEST_F(TracezDataAggregatorTest, SpanNameInAlphabeticalOrder)
{
  std::vector<std::string> span_names = {span_name1, span_name2, span_name3};

  auto span_first = tracer->StartSpan(span_name2);
  tracer->StartSpan(span_name1)->End();
  auto span_third = tracer->StartSpan(span_name3);
  span_third->SetStatus(opentelemetry::trace::StatusCode::kError, "span cancelled");
  span_third->End();
  std::this_thread::sleep_for(milliseconds(500));
  // Get data and check if span name exists in aggregation
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), span_names.size());

  int span_names_idx = 0;
  for (auto &spans : data)
  {
    ASSERT_EQ(spans.first, span_names[span_names_idx]);
    span_names_idx++;
  }
  span_first->End();
}

/** This test checks to see that there is no double counting of running spans
 * when get aggregated data is called twice**/
TEST_F(TracezDataAggregatorTest, AdditionToRunningSpans)
{
  // Start a span and check the data
  auto span_first = tracer->StartSpan(span_name1);
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  VerifySpanCountsInTracezData(span_name1, data.at(span_name1), 1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  // Start another span and check to see if there is no double counting of spans
  auto span_second = tracer->StartSpan(span_name1);

  // Give some time and get updated data
  std::this_thread::sleep_for(milliseconds(500));
  data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 2, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  ASSERT_EQ(aggregated_data.sample_running_spans.size(), 2);
  for (auto &sample_span : aggregated_data.sample_running_spans)
  {
    ASSERT_EQ(sample_span.GetName().data(), span_name1);
  }
  span_first->End();
  span_second->End();
}

/** This test checks to see that once a running span is completed it the
 * aggregated data is updated correctly **/
TEST_F(TracezDataAggregatorTest, RemovalOfRunningSpanWhenCompleted)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(nanoseconds(40));

  // Start a span and make sure data is updated
  auto span_first = tracer->StartSpan(span_name1, start);
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  VerifySpanCountsInTracezData(span_name1, data.at(span_name1), 1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_EQ(data.at(span_name1).sample_running_spans.front().GetName().data(), span_name1);
  // End the span and make sure running span is removed and completed span is
  // updated, there should be only one completed span
  span_first->End(end);
  std::this_thread::sleep_for(milliseconds(500));

  // Make sure sample span still exists before next aggregation
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1).sample_running_spans.front().GetName().data(), span_name1);

  data = tracez_data_aggregator->GetAggregatedTracezData();

  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  // Check if completed span fields are correctly updated
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, 0, {1, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_EQ(aggregated_data.sample_latency_spans[LatencyBoundary::k0MicroTo10Micro]
                .front()
                .GetDuration()
                .count(),
            30);
}

TEST_F(TracezDataAggregatorTest, RunningSpanChangesNameBeforeCompletion)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(10));
  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(nanoseconds(40));

  // Start a span and make sure data is updated
  auto span_first = tracer->StartSpan(span_name1, start);
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());
  VerifySpanCountsInTracezData(span_name1, data.at(span_name1), 1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_EQ(data.at(span_name1).sample_running_spans.front().GetName().data(), span_name1);

  // End the span and make sure running span is removed and completed span is
  // updated, there should be only one completed span
  span_first->UpdateName(span_name2);
  span_first->End(end);

  // Check if sample span is present before fetching updated data
  std::this_thread::sleep_for(milliseconds(500));
  ASSERT_TRUE(data.find(span_name1) != data.end());
  ASSERT_EQ(data.at(span_name1).sample_running_spans.front().GetName(), span_name1);

  data = tracez_data_aggregator->GetAggregatedTracezData();

  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name2) != data.end());

  // Check if completed span fields are correctly updated
  auto &aggregated_data = data.at(span_name2);
  VerifySpanCountsInTracezData(span_name2, aggregated_data, 0, 0, {1, 0, 0, 0, 0, 0, 0, 0, 0});
  ASSERT_EQ(aggregated_data.sample_latency_spans[LatencyBoundary::k0MicroTo10Micro]
                .front()
                .GetDuration()
                .count(),
            30);
}

/** Test to check if the span latencies with duration at the edge of boundaries
 * fall in the correct bucket **/
TEST_F(TracezDataAggregatorTest, EdgeSpanLatenciesFallInCorrectBoundaries)
{
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;

  // Start and end 6 spans with the same name that fall into the first latency
  // bucket
  std::vector<nanoseconds> durations = {
      nanoseconds(0),          nanoseconds(10000),       nanoseconds(100000),
      nanoseconds(1000000),    nanoseconds(10000000),    nanoseconds(100000000),
      nanoseconds(1000000000), nanoseconds(10000000000), nanoseconds(100000000000)};
  for (auto duration : durations)
  {
    start.start_steady_time = SteadyTimestamp(nanoseconds(1));
    end.end_steady_time     = SteadyTimestamp(nanoseconds(duration.count() + 1));
    tracer->StartSpan(span_name1, start)->End(end);
  }

  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_EQ(data.size(), 1);
  ASSERT_TRUE(data.find(span_name1) != data.end());

  std::this_thread::sleep_for(milliseconds(500));
  auto &aggregated_data = data.at(span_name1);
  VerifySpanCountsInTracezData(span_name1, aggregated_data, 0, 0, {1, 1, 1, 1, 1, 1, 1, 1, 1});

  // Check if the latency boundary is updated correctly
  for (unsigned int boundary = 0; boundary < kLatencyBoundaries.size(); boundary++)
  {
    ASSERT_EQ(aggregated_data.sample_latency_spans[boundary].front().GetDuration().count(),
              durations[boundary].count());
  }
}

/** This test makes sure that the data is consistent when there are multiple
 * calls to the data aggegator with no change in data **/
TEST_F(TracezDataAggregatorTest, NoChangeInBetweenCallsToAggregator)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(1));

  opentelemetry::trace::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(nanoseconds(1));

  tracer->StartSpan(span_name1, start)->End(end);
  auto running_span = tracer->StartSpan(span_name2);
  auto span         = tracer->StartSpan(span_name3);
  span->SetStatus(opentelemetry::trace::StatusCode::kError, "span cancelled");
  span->End();
  std::this_thread::sleep_for(milliseconds(500));
  auto data = tracez_data_aggregator->GetAggregatedTracezData();
  std::this_thread::sleep_for(milliseconds(500));
  // Get data and check if span name exists in aggregation
  data = tracez_data_aggregator->GetAggregatedTracezData();
  ASSERT_TRUE(data.find(span_name1) != data.end());
  VerifySpanCountsInTracezData(span_name1, data.at(span_name1), 0, 0, {1, 0, 0, 0, 0, 0, 0, 0, 0});

  ASSERT_TRUE(data.find(span_name2) != data.end());
  VerifySpanCountsInTracezData(span_name2, data.at(span_name2), 1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  ASSERT_TRUE(data.find(span_name3) != data.end());
  VerifySpanCountsInTracezData(span_name3, data.at(span_name3), 0, 1, {0, 0, 0, 0, 0, 0, 0, 0, 0});

  running_span->End();
}
