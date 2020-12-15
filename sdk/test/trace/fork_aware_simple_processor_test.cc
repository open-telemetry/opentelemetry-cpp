#ifdef __unix__
#  include <gtest/gtest.h>
#  include <sys/mman.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/fork_aware_exporter.h"
#include "opentelemetry/sdk/trace/fork_aware_simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

using namespace opentelemetry::sdk::trace;

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public ForkAwareSpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<bool> span_received,
                   std::shared_ptr<bool> shutdown_called) noexcept
      : span_received_(span_received), shutdown_called_(shutdown_called)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &spans) noexcept override
  {
    for (auto &span : spans)
    {
      if (span != nullptr)
      {
        *span_received_ = true;
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    *shutdown_called_ = true;
  }

  void OnForkedChild() noexcept
  {
    *span_received_   = false;
    *shutdown_called_ = false;
  }

private:
  std::shared_ptr<bool> span_received_;
  std::shared_ptr<bool> shutdown_called_;
};

#ifdef __unix__
TEST(ForkAwareSimpleSpanProcessor, ToMockSpanExporter)
{
  // Create shared memory
  bool *did_child_tests_pass = static_cast<bool *>(
      mmap(nullptr, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  *did_child_tests_pass = false;

  std::shared_ptr<bool> span_received(new bool(false));
  std::shared_ptr<bool> shutdown_called(new bool(false));
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(span_received, shutdown_called));
  ForkAwareSimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable);
  ASSERT_FALSE(*span_received);

  processor.OnEnd(std::move(recordable));
  ASSERT_TRUE(*span_received);

  pid_t child_pid = fork();

  if (child_pid == 0)
  {
    *span_received = false;

    auto recordable = processor.MakeRecordable();

    processor.OnStart(*recordable);
    *did_child_tests_pass = *span_received == false;

    processor.OnEnd(std::move(recordable));
    *did_child_tests_pass &= *span_received == true;

    processor.Shutdown();
    *did_child_tests_pass &= *shutdown_called == true;

    std::exit(0);
  }

  processor.Shutdown();
  ASSERT_TRUE(*shutdown_called);

  // Wait for child process to finish
  waitpid(child_pid, nullptr, 0);

  EXPECT_TRUE(*did_child_tests_pass);

  munmap(did_child_tests_pass, sizeof(bool));
}
#endif