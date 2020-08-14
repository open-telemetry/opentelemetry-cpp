#ifdef __unix__
#  include <sys/mman.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/sdk/trace/fork_aware_batch_span_processor.h"
#include "opentelemetry/sdk/trace/fork_aware_exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Returns a mock span exporter meant exclusively for testing only
 */
class MockSpanExporter final : public ForkAwareSpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
                   std::shared_ptr<std::atomic<bool>> is_export_completed) noexcept
      : spans_received_(spans_received), is_export_completed_(is_export_completed)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    *is_export_completed_ = false;

    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));

      if (span != nullptr)
      {
        spans_received_->push_back(std::move(span));
      }
    }

    *is_export_completed_ = true;
    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {}

  void OnForkedChild() noexcept
  {
    // Clear all spans in vector and reset boolean flag(s)
    spans_received_->clear();
    *is_export_completed_ = false;
  }

private:
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_;
  std::shared_ptr<std::atomic<bool>> is_export_completed_;
};

/**
 * Fixture Class
 */
class ForkAwareBatchSpanProcessorTestPeer : public testing::Test
{
public:
  std::shared_ptr<SpanProcessor> GetMockProcessor(
      std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
      std::shared_ptr<std::atomic<bool>> is_export_completed =
          std::shared_ptr<std::atomic<bool>>(new std::atomic<bool>(false)),
      const std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000),
      const size_t max_queue_size                           = 2048,
      const size_t max_export_batch_size                    = 512)
  {
    return std::shared_ptr<SpanProcessor>(new ForkAwareBatchSpanProcessor(
        GetMockExporter(spans_received, is_export_completed), max_queue_size, schedule_delay_millis,
        max_export_batch_size));
  }

private:
  std::unique_ptr<SpanExporter> GetMockExporter(
      std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
      std::shared_ptr<std::atomic<bool>> is_export_completed)
  {
    return std::unique_ptr<SpanExporter>(new MockSpanExporter(spans_received, is_export_completed));
  }
};

#ifdef __unix__
#  if !defined(__SANITIZE_THREAD__) && !defined(__SANITIZE_ADDRESS__)
TEST_F(ForkAwareBatchSpanProcessorTestPeer, TestForkHandlers)
{
  // Create shared memory
  bool *did_child_tests_pass = static_cast<bool *>(
      mmap(nullptr, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  *did_child_tests_pass = false;

  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  const std::chrono::milliseconds schedule_delay_millis(1000);

  auto batch_processor =
      GetMockProcessor(spans_received, is_export_completed, schedule_delay_millis);

  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdk::trace::TracerProvider(batch_processor));

  auto tracer = provider->GetTracer("Test tracer");

  // Create a span in the parent process
  auto span = tracer->StartSpan("Span");
  span->End();

  std::this_thread::sleep_for(schedule_delay_millis + std::chrono::milliseconds(50));

  // The span in the parent process will now be exported
  EXPECT_TRUE(is_export_completed->load());

  pid_t child_pid = fork();

  if (child_pid == 0)
  {
    // CHILD PROCESS

    // Generate some spans
    auto span_1 = tracer->StartSpan("Span_1");
    span_1->End();
    auto span_2 = tracer->StartSpan("Span_2");
    span_2->End();

    std::this_thread::sleep_for(schedule_delay_millis + std::chrono::milliseconds(100));

    // Spans should now be exported in the child process
    *did_child_tests_pass = is_export_completed->load() == true && spans_received->size() == 2;

    batch_processor->Shutdown();

    // End child process
    std::exit(0);
  }

  // We only created one span in the parent process
  EXPECT_EQ(1, spans_received->size());

  // Wait for child process to finish
  waitpid(child_pid, nullptr, 0);

  EXPECT_TRUE(*did_child_tests_pass);

  munmap(did_child_tests_pass, sizeof(bool));
}
#  endif
#endif

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
