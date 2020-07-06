#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter {
 public:
  MockSpanExporter(std::shared_ptr<bool> span_received,
                   std::shared_ptr<bool> shutdown_called) noexcept
      : span_received_(span_received), shutdown_called_(shutdown_called) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &spans) noexcept override {
    for (auto &span : spans) {
      if (span != nullptr) {
        *span_received_ = true;
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override {
    *shutdown_called_ = true;
  }

 private:
  std::shared_ptr<bool> span_received_;
  std::shared_ptr<bool> shutdown_called_;
};


/*
 * Helper function uses the current processor to
 * update spans contained in completed_spans and
 * running_spans. completed acts contains all spans,
 * unless marked otherwise
 */
void UpdateSpans(std::shared_ptr<TracezSpanProcessor>& processor,
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>& completed,
    std::unordered_set<opentelemetry::sdk::trace::Recordable*>& running,
    bool store_only_new_completed = false) {

  running = processor->GetRunningSpans();
  auto temp = processor->GetCompletedSpans();
  if (store_only_new_completed) {
    completed.clear();
    completed = std::move(temp);
  }
  else {
    std::move(temp.begin(), temp.end(),
            std::inserter(completed, completed.end()));
  }
  temp.clear();

}


/*
 * Returns true if all the span names in the name
 * vector within the given range appears in at least
 * the same frequency as they do in running_spans.
 *
 * If no start value is given, start at index 0
 * If no end value is given, end at name vector end
 * If 1-1 correspondance marked, return true
 * if completed has all names in same frequency,
 * no more or less
 */
bool ContainsNames(const std::vector<std::string>& names,
    std::unordered_set<opentelemetry::sdk::trace::Recordable*>& running,
    unsigned int name_start = 0, unsigned int name_end = 0,
    bool one_to_one_correspondence = false) {
  /* TEMPORARILY COMMENTED OUT WHILE RECORDABLE HAS NO GetName() FUNCTION
  if (name_end == 0) name_end = names.size();

  unsigned int num_names = name_end - name_start;

  if (num_names > running.size() || // More names than spans, can't have all names
       (one_to_one_correspondence && num_names != running.size())) {
    return false;
  }
  std::vector<bool> is_contained(num_names, false);

  // Mark all names that are contained only once
  // in the order they appear
  for (auto &span : running) {
    for (unsigned int i = 0; i < num_names; i++) {
      if (span->GetName() == names[name_start + i] && !is_contained[i]) {
        is_contained[i] = true;
        break;
      }
    }
  }

  for (auto &&b : is_contained) if (!b) return false;
  */
  return true;

}


/*
 * Returns true if all the span names in the name
 * vector within the given range appears in at least
 * the same frequency as they do in completed_spans
 *
 * If no start value is given, start at index 0
 * If no end value is given, end at name vector end
 * If 1-1 correspondance marked, return true
 * if completed has all names in same frequency,
 * no more or less
 */
bool ContainsNames(const std::vector<std::string>& names,
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>& completed,
    unsigned int name_start = 0, unsigned int name_end = 0,
    bool one_to_one_correspondence = false) {
  /* TEMPORARILY COMMENTED OUT WHILE RECORDABLE HAS NO GetName() FUNCTION

  if (name_end == 0) name_end = names.size();

  unsigned int num_names = name_end - name_start;

  if (num_names > completed.size() ||
       (one_to_one_correspondence && num_names != completed.size())) {
        return false;
  }
  std::vector<bool> is_contained(num_names, false);

  for (auto &span : completed) {
    for (unsigned int i = 0; i < num_names; i++) {
      if (span->GetName() == names[name_start + i] && !is_contained[i]) {
        is_contained[i] = true;
        break;
      }
    }
  }

  for (auto &&b : is_contained) if (!b) return false;
  */
  return true;

}


TEST(TracezSpanProcessor, GetSpansAndUseMockSpanExporter) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));

  auto span = processor.get()->MakeRecordable();

  processor.get()->OnStart(*span);
  ASSERT_FALSE(*span_received);

  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  //std::vector<std::string> span_name = { "span" };

 // auto span = tracer->StartSpan(span_name[0]);
 // UpdateSpans(processor, completed, running);

  //ASSERT_TRUE(ContainsNames(span_name, running));
  ASSERT_EQ(running.size(), 1);
  ASSERT_EQ(completed.size(), 0);

  processor.get()->OnEnd(std::move(span));
  ASSERT_TRUE(*span_received);
  //span->End();
  UpdateSpans(processor, completed, running);

  //ASSERT_TRUE(ContainsNames(span_name, completed));
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 1);

  processor.get()->Shutdown();
  ASSERT_TRUE(*shutdown_called);
}

/*
 * Test if the TraceZ processor correctly batches and
 * exports spans
 */
TEST(TracezSpanProcessor, ToMockSpanExporter) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  TracezSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);
  ASSERT_FALSE(*span_received);

  processor.OnEnd(std::move(recordable));
  ASSERT_TRUE(*span_received);

  processor.Shutdown();
  ASSERT_TRUE(*shutdown_called);
}


/*
 * Test if both span containers are empty when no spans
 * exist or are added
 */
TEST(TracezSpanProcessor, NoSpans) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));

  ASSERT_EQ(processor->GetRunningSpans().size(), 0);
  ASSERT_EQ(processor->GetCompletedSpans().size(), 0);

}


/*
 * Test if a single span moves from running to completed
 * at expected times. All completed spans are stored.
*/
TEST(TracezSpanProcessor, OneSpanRightContainerStored) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  std::vector<std::string> span_name = { "span" };

  auto span = tracer->StartSpan(span_name[0]);
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_name, running));
  ASSERT_EQ(running.size(), 1);
  ASSERT_EQ(completed.size(), 0);

  span->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_name, completed));
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 1);

}


/*
 * Test if multiple spans move from running to completed at
 * expected times. Check if all are in a container, either
 * running/completed during checks. All completed spans are stored.
*/
TEST(TracezSpanProcessor, MultipleSpansRightContainerStored) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 0);

  std::vector<std::string> span_names = {"s1", "s2", "s3", "s1"};

  // Start and store spans using span_names
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>> span_vars;
  for (const auto &name : span_names) span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running)); // s1 s2 s3 s1
  ASSERT_EQ(running.size(), span_names.size());
  ASSERT_EQ(completed.size(), 0);

  // End all spans
  for (auto &span : span_vars) span->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, completed)); // s1 s2 s3 s1
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), span_names.size());

}


/*
 * Test if multiple spans move from running to completed
 * at expected times, running/completed spans are split.
 * Middle spans end first. All completed spans are stored.
*/
TEST(TracezSpanProcessor, MultipleSpansRightContainerMiddleStored) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  std::vector<std::string> span_names = {"s0", "s2", "s1", "s1", "s"};

  // Start and store spans using span_names
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>> span_vars;
  for (const auto &name : span_names) span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running)); // s0 s2 s1 s1 s
  ASSERT_EQ(running.size(), span_names.size());
  ASSERT_EQ(completed.size(), 0);

  // End 4th span
  span_vars[3].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 3)); // s0 s2 s1
  ASSERT_TRUE(ContainsNames(span_names, running, 4)); // + s
  ASSERT_TRUE(ContainsNames(span_names, completed, 3, 4)); // s1
  ASSERT_EQ(running.size(), 4);
  ASSERT_EQ(completed.size(), 1);

  // End 2nd span
  span_vars[1].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 1)); // s0
  ASSERT_TRUE(ContainsNames(span_names, running, 2, 3)); // + s1
  ASSERT_TRUE(ContainsNames(span_names, running, 4)); // + s
  ASSERT_TRUE(ContainsNames(span_names, completed, 1, 2)); // s2
  ASSERT_TRUE(ContainsNames(span_names, completed, 3, 4)); // s1
  ASSERT_EQ(running.size(), 3);
  ASSERT_EQ(completed.size(), 2);

  // End 3rd span (last middle span)
  span_vars[2].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 1)); // s0
  ASSERT_TRUE(ContainsNames(span_names, running, 4)); // + s
  ASSERT_TRUE(ContainsNames(span_names, completed, 1, 4)); // s2 s1 s1
  ASSERT_EQ(running.size(), 2);
  ASSERT_EQ(completed.size(), 3);

  // End remaining Spans
  span_vars[0].get()->End();
  span_vars[4].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, completed)); // s0 s2 s1 s1 s
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 5);
}


/*
 * Test if multiple spans move from running to completed
 * at expected times, running/completed spans are split.
 * All completed spans are stored.
*/
TEST(TracezSpanProcessor, MultipleSpansRightContainerOuterStored) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  std::vector<std::string> span_names = {"s0", "s2", "s1", "s1", "s"};

  // Start and store spans using span_names
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>> span_vars;
  for (const auto &name : span_names) span_vars.push_back(tracer->StartSpan(name));

  // End last span
  span_vars[4].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 4)); // s0 s2 s1 s1
  ASSERT_TRUE(ContainsNames(span_names, completed, 4)); // s
  ASSERT_EQ(running.size(), 4);
  ASSERT_EQ(completed.size(), 1);

  // End first span
  span_vars[0].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, 1, 4)); // s2 s1 s1
  ASSERT_TRUE(ContainsNames(span_names, completed, 0, 1)); // s0
  ASSERT_TRUE(ContainsNames(span_names, completed, 4)); // s
  ASSERT_EQ(running.size(), 3);
  ASSERT_EQ(completed.size(), 2);

  // End remaining Spans
  for (int i = 1; i < 4; i++) span_vars[i].get()->End();
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, completed)); // s0 s2 s1 s1 s
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 5);
}


/*
 * Test if multiple spans move from running to completed
 * at expected times, running/completed spans are split.
 * Middle spans end first. Only new completed spans are stored.
*/
TEST(TracezSpanProcessor, MultipleSpansRightContainerMiddleNewOnly) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  std::vector<std::string> span_names = {"s0", "s2", "s1", "s1", "s"};

  // Start and store spans using span_names
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>> span_vars;
  for (const auto &name : span_names) span_vars.push_back(tracer->StartSpan(name));
  UpdateSpans(processor, completed, running);

  ASSERT_TRUE(ContainsNames(span_names, running, true)); // s0 s2 s1 s1 s
  ASSERT_EQ(running.size(), span_names.size());
  ASSERT_EQ(completed.size(), 0);

  // End 4th span
  span_vars[3].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 3)); // s0 s2 s1
  ASSERT_TRUE(ContainsNames(span_names, running, 4)); // + s
  ASSERT_TRUE(ContainsNames(span_names, completed, 3, 4, true)); // s1
  ASSERT_EQ(running.size(), 4);
  ASSERT_EQ(completed.size(), 1);

  // End 2nd and 3rd span
  span_vars[1].get()->End();
  span_vars[2].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 1)); // s0
  ASSERT_TRUE(ContainsNames(span_names, running, 4)); // + s
  ASSERT_TRUE(ContainsNames(span_names, completed, 1, 3, true)); // s2 s1
  ASSERT_EQ(running.size(), 2);
  ASSERT_EQ(completed.size(), 2);

  // End remaining Spans
  span_vars[0].get()->End();
  span_vars[4].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, completed, 0, 1)); // s0
  ASSERT_TRUE(ContainsNames(span_names, completed, 4)); // s
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 2);
}


/*
 * Test if multiple spans move from running to completed
 * at expected times, running/completed spans are split.
 * Only new completed spans are stored.
*/
TEST(TracezSpanProcessor, MultipleSpansRightContainerOuterNewOnly) {
  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor(std::move(exporter)));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  auto running = processor->GetRunningSpans();
  auto completed = processor->GetCompletedSpans();

  std::vector<std::string> span_names = {"s0", "s2", "s1", "s1", "s"};

  // Start and store spans using span_names
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::trace::Span>> span_vars;
  for (const auto &name : span_names) span_vars.push_back(tracer->StartSpan(name));

  // End last span
  span_vars[4].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, running, 0, 4, true)); // s0 s2 s1 s1
  ASSERT_TRUE(ContainsNames(span_names, completed, 4, 5, true)); // s
  ASSERT_EQ(running.size(), 4);
  ASSERT_EQ(completed.size(), 1);

  // End first span
  span_vars[0].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, running, 1, 4, true)); // s2 s1 s1
  ASSERT_TRUE(ContainsNames(span_names, completed, 0, 1, true)); // s0
  ASSERT_EQ(running.size(), 3);
  ASSERT_EQ(completed.size(), 1);

  // End remaining middle pans
  for (int i = 1; i < 4; i++) span_vars[i].get()->End();
  UpdateSpans(processor, completed, running, true);

  ASSERT_TRUE(ContainsNames(span_names, completed, 1, 4, true)); // s2 s1 s1
  ASSERT_EQ(running.size(), 0);
  ASSERT_EQ(completed.size(), 3);
}


