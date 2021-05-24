// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_processor.h"

#include <gtest/gtest.h>

#include <thread>

#include "opentelemetry/ext/zpages/threadsafe_span_data.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;

//////////////////////////////////// TEST HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function uses the current processor to update spans contained in completed_spans
 * and running_spans. completed_spans contains all spans (cumulative), unless marked otherwise
 */
void UpdateSpans(std::shared_ptr<TracezSharedData> &data,
                 std::vector<std::unique_ptr<ThreadsafeSpanData>> &completed,
                 std::unordered_set<ThreadsafeSpanData *> &running,
                 bool store_only_new_completed = false)
{
  auto spans = data->GetSpanSnapshot();
  running    = spans.running;
  if (store_only_new_completed)
  {
    completed.clear();
    completed = std::move(spans.completed);
  }
  else
  {
    std::move(spans.completed.begin(), spans.completed.end(),
              std::inserter(completed, completed.end()));
  }
  spans.completed.clear();
}

/*
 * Returns true if all the span names in the name vector within the given range appears in
 * at least the same frequency as they do in running_spans.
 *
 * If no start value is given, start at index 0
 * If no end value is given, end at name vector end
 * If 1-1 correspondance marked, return true if completed has all names in same frequency,
 * no more or less
 */
bool ContainsNames(const std::vector<std::string> &names,
                   std::unordered_set<ThreadsafeSpanData *> &running,
                   size_t name_start              = 0,
                   size_t name_end                = 0,
                   bool one_to_one_correspondence = false)
{
  if (name_end == 0)
    name_end = names.size();

  size_t num_names = name_end - name_start;

  if (num_names > running.size() ||  // More names than spans, can't have all names
      (one_to_one_correspondence && num_names != running.size()))
  {
    return false;
  }
  std::vector<bool> is_contained(num_names, false);

  // Mark all names that are contained only once
  // in the order they appear
  for (auto &span : running)
  {
    for (unsigned int i = 0; i < num_names; i++)
    {
      if (span->GetName() == names[name_start + i] && !is_contained[i])
      {
        is_contained[i] = true;
        break;
      }
    }
  }

  for (auto &&b : is_contained)
    if (!b)
      return false;

  return true;
}

/*
 * Returns true if all the span names in the nam vector within the given range appears in
 * at least the same frequency as they do in completed_spans
 *
 * If no start value is given, start at index 0
 * If no end value is given, end at name vector end
 * If 1-1 correspondance marked, return true if completed has all names in same frequency,
 * no more or less
 */
bool ContainsNames(const std::vector<std::string> &names,
                   std::vector<std::unique_ptr<ThreadsafeSpanData>> &completed,
                   size_t name_start              = 0,
                   size_t name_end                = 0,
                   bool one_to_one_correspondence = false)
{

  if (name_end == 0)
    name_end = names.size();

  size_t num_names = name_end - name_start;

  if (num_names > completed.size() || (one_to_one_correspondence && num_names != completed.size()))
  {
    return false;
  }
  std::vector<bool> is_contained(num_names, false);

  for (auto &span : completed)
  {
    for (unsigned int i = 0; i < num_names; i++)
    {
      if (span->GetName() == names[name_start + i] && !is_contained[i])
      {
        is_contained[i] = true;
        break;
      }
    }
  }

  for (auto &&b : is_contained)
    if (!b)
      return false;

  return true;
}

/*
 * Helper function calls GetSpanSnapshot() i times and does nothing with it
 * otherwise. Used for testing thread safety
 */
void GetManySnapshots(std::shared_ptr<TracezSharedData> &data, int i)
{
  for (; i > 0; i--)
    data->GetSpanSnapshot();
}

/*
 * Helper function that creates i spans, which are added into the passed
 * in vector. Used for testing thread safety
 */
void StartManySpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i)
{
  for (; i > 0; i--)
    spans.push_back(tracer->StartSpan("span"));
}

/*
 * Helper function that ends all spans in the passed in span vector. Used
 * for testing thread safety
 */
void EndAllSpans(std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans)
{
  for (auto &span : spans)
    span->End();
}

//////////////////////////////// TEST FIXTURE //////////////////////////////////////

/*
 * Reduce code duplication by having single area with shared setup code
 */
class TracezProcessor : public ::testing::Test
{
protected:
  void SetUp() override
  {
    shared_data = std::shared_ptr<TracezSharedData>(new TracezSharedData());
    processor   = std::shared_ptr<TracezSpanProcessor>(new TracezSpanProcessor(shared_data));
    std::unique_ptr<SpanProcessor> processor2(new TracezSpanProcessor(shared_data));
    std::vector<std::unique_ptr<SpanProcessor>> processors;
    processors.push_back(std::move(processor2));
    auto resource = opentelemetry::sdk::resource::Resource::Create({});

    // Note: we make a *different* processor for the tracercontext. THis is because
    // all the tests use shared data, and we want to make sure this works correctly.
    auto context = std::make_shared<TracerContext>(std::move(processors), resource);

    tracer     = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(context));
    auto spans = shared_data->GetSpanSnapshot();
    running    = spans.running;
    completed  = std::move(spans.completed);

    span_names = {"s0", "s2", "s1", "s1", "s"};
  }

  std::shared_ptr<TracezSharedData> shared_data;
  std::shared_ptr<TracezSpanProcessor> processor;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;

  std::vector<std::string> span_names;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> span_vars;

  std::unordered_set<ThreadsafeSpanData *> running;
  std::vector<std::unique_ptr<ThreadsafeSpanData>> completed;
};

///////////////////////////////////////// TESTS ///////////////////////////////////

/*
 * Test if both span containers are empty when no spans exist or are added.
 * Ensures no rogue spans appear in the containers somehow.
 */
TEST_F(TracezProcessor, NoSpans)
{
  auto recordable = processor->MakeRecordable();

  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 0);
}

/*
 * Test if a single span moves from running to completed at expected times.
 * All completed spans are stored. Ensures basic functionality and that accumulation
 * can happen
 */
TEST_F(TracezProcessor, OneSpanCumulative)
{
  auto span = tracer->StartSpan(span_names[0]);
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 1, true));
  EXPECT_EQ(running.size(), 1);
  EXPECT_EQ(completed.size(), 0);

  span->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, completed, 0, 1, true));
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 1);
}

/*
 * Test if multiple spans move from running to completed at  expected times. Check if
 * all are in a container, either running/completed during checks. Ensures basic functionality
 * and that accumulation can happen for many spans
 * All completed spans are stored.
 */
TEST_F(TracezProcessor, MultipleSpansCumulative)
{
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 0);

  // Start and store spans using span_names
  for (const auto &name : span_names)
    span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), span_names.size());
  EXPECT_EQ(completed.size(), 0);

  // End all spans
  for (auto &span : span_vars)
    span->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, completed));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), span_names.size());
}

/*
 * Test if multiple spans move from running to completed at expected times,
 * running/completed spans are split. Middle spans end first. Ensures basic functionality
 * and that accumulation can happen for many spans even spans that start and end non-
 * sequentially. All completed spans are stored.
 */
TEST_F(TracezProcessor, MultipleSpansMiddleSplitCumulative)
{
  for (const auto &name : span_names)
    span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), span_names.size());
  EXPECT_EQ(completed.size(), 0);

  // End 4th span
  span_vars[3]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 3));    // s0 s2 s1
  EXPECT_TRUE(ContainsNames(span_names, running, 4));       // + s
  EXPECT_TRUE(ContainsNames(span_names, completed, 3, 4));  // s1
  EXPECT_EQ(running.size(), 4);
  EXPECT_EQ(completed.size(), 1);

  // End 2nd span
  span_vars[1]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 1));    // s0
  EXPECT_TRUE(ContainsNames(span_names, running, 2, 3));    // + s1
  EXPECT_TRUE(ContainsNames(span_names, running, 4));       // + s
  EXPECT_TRUE(ContainsNames(span_names, completed, 1, 2));  // s2
  EXPECT_TRUE(ContainsNames(span_names, completed, 3, 4));  // s1
  EXPECT_EQ(running.size(), 3);
  EXPECT_EQ(completed.size(), 2);

  // End 3rd span (last middle span)
  span_vars[2]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 1));    // s0
  EXPECT_TRUE(ContainsNames(span_names, running, 4));       // + s
  EXPECT_TRUE(ContainsNames(span_names, completed, 1, 4));  // s2 s1 s1
  EXPECT_EQ(running.size(), 2);
  EXPECT_EQ(completed.size(), 3);

  // End remaining Spans
  span_vars[0]->End();
  span_vars[4]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, completed));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 5);
}

/*
 * Test if multiple spans move from running to completed at expected times,
 * running/completed spans are split.  Ensures basic functionality and that
 * accumulation can happen for many spans even spans that start and end non-
 * sequentially. All completed spans are stored.
 */
TEST_F(TracezProcessor, MultipleSpansOuterSplitCumulative)
{
  for (const auto &name : span_names)
    span_vars.push_back(tracer->StartSpan(name));

  // End last span
  span_vars[4]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 4));  // s0 s2 s1 s1
  EXPECT_TRUE(ContainsNames(span_names, completed, 4));   // s
  EXPECT_EQ(running.size(), 4);
  EXPECT_EQ(completed.size(), 1);

  // End first span
  span_vars[0]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 1, 4));    // s2 s1 s1
  EXPECT_TRUE(ContainsNames(span_names, completed, 0, 1));  // s0
  EXPECT_TRUE(ContainsNames(span_names, completed, 4));     // s
  EXPECT_EQ(running.size(), 3);
  EXPECT_EQ(completed.size(), 2);

  // End remaining Spans
  for (int i = 1; i < 4; i++)
    span_vars[i]->End();
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, completed));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 5);
}

/*
 * Test if a single span moves from running to completed at expected times.
 * Ensure correct behavior even when spans are discarded. Only new completed
 * spans are stored.
 */
TEST_F(TracezProcessor, OneSpanNewOnly)
{
  auto span = tracer->StartSpan(span_names[0]);
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 1, true));
  EXPECT_EQ(running.size(), 1);
  EXPECT_EQ(completed.size(), 0);

  span->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, completed, 0, 1, true));
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 1);

  UpdateSpans(shared_data, completed, running, true);

  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 0);
}

/*
 * Test if multiple spans move from running to completed at expected times,
 * running/completed spans are split. Middle spans end first. Ensure correct
 * behavior even when multiple spans are discarded, even when span starting and
 * ending is non-sequential. Only new completed spans are stored.
 */
TEST_F(TracezProcessor, MultipleSpansMiddleSplitNewOnly)
{
  for (const auto &name : span_names)
    span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(shared_data, completed, running);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 5, true));  // s0 s2 s1 s1 s
  EXPECT_EQ(running.size(), span_names.size());
  EXPECT_EQ(completed.size(), 0);

  // End 4th span
  span_vars[3]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 3));          // s0 s2 s1
  EXPECT_TRUE(ContainsNames(span_names, running, 4));             // + s
  EXPECT_TRUE(ContainsNames(span_names, completed, 3, 4, true));  // s1
  EXPECT_EQ(running.size(), 4);
  EXPECT_EQ(completed.size(), 1);

  // End 2nd and 3rd span
  span_vars[1]->End();
  span_vars[2]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 1));          // s0
  EXPECT_TRUE(ContainsNames(span_names, running, 4));             // + s
  EXPECT_TRUE(ContainsNames(span_names, completed, 1, 3, true));  // s2 s1
  EXPECT_EQ(running.size(), 2);
  EXPECT_EQ(completed.size(), 2);

  // End remaining Spans
  span_vars[0]->End();
  span_vars[4]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, completed, 0, 1));  // s0
  EXPECT_TRUE(ContainsNames(span_names, completed, 4));     // s
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 2);

  UpdateSpans(shared_data, completed, running, true);

  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 0);
}

/*
 * Test if multiple spans move from running to completed at expected times,
 * running/completed spans are split. Ensure correct behavior even when
 * multiple spans are discarded, even when span starting and ending is
 * non-sequential. Only new completed spans are stored.
 */
TEST_F(TracezProcessor, MultipleSpansOuterSplitNewOnly)
{
  for (const auto &name : span_names)
    span_vars.push_back(tracer->StartSpan(name));

  // End last span
  span_vars[4]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, running, 0, 4, true));    // s0 s2 s1 s1
  EXPECT_TRUE(ContainsNames(span_names, completed, 4, 5, true));  // s
  EXPECT_EQ(running.size(), 4);
  EXPECT_EQ(completed.size(), 1);

  // End first span
  span_vars[0]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, running, 1, 4, true));    // s2 s1 s1
  EXPECT_TRUE(ContainsNames(span_names, completed, 0, 1, true));  // s0
  EXPECT_EQ(running.size(), 3);
  EXPECT_EQ(completed.size(), 1);

  // End remaining middle spans
  for (int i = 1; i < 4; i++)
    span_vars[i]->End();
  UpdateSpans(shared_data, completed, running, true);

  EXPECT_TRUE(ContainsNames(span_names, completed, 1, 4, true));  // s2 s1 s1
  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 3);

  UpdateSpans(shared_data, completed, running, true);

  EXPECT_EQ(running.size(), 0);
  EXPECT_EQ(completed.size(), 0);
}

/*
 * Test for ForceFlush and Shutdown code coverage, which do nothing.
 */
TEST_F(TracezProcessor, FlushShutdown)
{
  auto pre_running_sz   = running.size();
  auto pre_completed_sz = completed.size();

  EXPECT_TRUE(processor->ForceFlush());
  EXPECT_TRUE(processor->Shutdown());

  UpdateSpans(shared_data, completed, running);

  EXPECT_EQ(pre_running_sz, running.size());
  EXPECT_EQ(pre_completed_sz, completed.size());
}

/*
 * Test for thread safety when many spans start at the same time.
 */
TEST_F(TracezProcessor, RunningThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans1;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

  std::thread start1(StartManySpans, std::ref(spans1), tracer, 500);
  std::thread start2(StartManySpans, std::ref(spans2), tracer, 500);

  start1.join();
  start2.join();

  EndAllSpans(spans1);
  EndAllSpans(spans2);
}

/*
 * Test for thread safety when many spans end at the same time
 */
TEST_F(TracezProcessor, CompletedThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans1;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

  StartManySpans(spans1, tracer, 500);
  StartManySpans(spans2, tracer, 500);

  std::thread end1(EndAllSpans, std::ref(spans1));
  std::thread end2(EndAllSpans, std::ref(spans2));

  end1.join();
  end2.join();
}

/*
 * Test for thread safety when many snapshots are grabbed at the same time.
 */
TEST_F(TracezProcessor, SnapshotThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans;

  std::thread snap1(GetManySnapshots, std::ref(shared_data), 500);
  std::thread snap2(GetManySnapshots, std::ref(shared_data), 500);

  snap1.join();
  snap2.join();

  StartManySpans(spans, tracer, 500);

  std::thread snap3(GetManySnapshots, std::ref(shared_data), 500);
  std::thread snap4(GetManySnapshots, std::ref(shared_data), 500);

  snap3.join();
  snap4.join();

  EndAllSpans(spans);
}

/*
 * Test for thread safety when many spans start while others are ending.
 */
TEST_F(TracezProcessor, RunningCompletedThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans1;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

  StartManySpans(spans1, tracer, 500);

  std::thread start(StartManySpans, std::ref(spans2), tracer, 500);
  std::thread end(EndAllSpans, std::ref(spans1));

  start.join();
  end.join();

  EndAllSpans(spans2);
}

/*
 * Test for thread safety when many span start while snapshots are being grabbed.
 */
TEST_F(TracezProcessor, RunningSnapshotThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans;

  std::thread start(StartManySpans, std::ref(spans), tracer, 500);
  std::thread snapshots(GetManySnapshots, std::ref(shared_data), 500);

  start.join();
  snapshots.join();

  EndAllSpans(spans);
}

/*
 * Test for thread safety when many spans end while snapshots are being grabbed.
 */
TEST_F(TracezProcessor, SnapshotCompletedThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans;

  StartManySpans(spans, tracer, 500);

  std::thread snapshots(GetManySnapshots, std::ref(shared_data), 500);
  std::thread end(EndAllSpans, std::ref(spans));

  snapshots.join();
  end.join();
}

/*
 * Test for thread safety when many spans start and end while snapshots are being grabbed.
 */
TEST_F(TracezProcessor, RunningSnapshotCompletedThreadSafety)
{
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans1;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

  StartManySpans(spans1, tracer, 500);

  std::thread start(StartManySpans, std::ref(spans2), tracer, 500);
  std::thread snapshots(GetManySnapshots, std::ref(shared_data), 500);
  std::thread end(EndAllSpans, std::ref(spans1));

  start.join();
  snapshots.join();
  end.join();

  EndAllSpans(spans2);
}
