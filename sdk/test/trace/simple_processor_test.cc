// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::sdk::common;
using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::exporter::memory::InMemorySpanExporter;
using opentelemetry::trace::SpanContext;

TEST(SimpleProcessor, ToInMemorySpanExporter)
{
  std::unique_ptr<InMemorySpanExporter> exporter(new InMemorySpanExporter());
  std::shared_ptr<InMemorySpanData> span_data = exporter->GetData();
  SimpleSpanProcessor processor(std::move(exporter));

  auto recordable = processor.MakeRecordable();

  processor.OnStart(*recordable, SpanContext::GetInvalid());

  ASSERT_EQ(0, span_data->GetSpans().size());

  processor.OnEnd(std::move(recordable));

  ASSERT_EQ(1, span_data->GetSpans().size());

  EXPECT_TRUE(processor.Shutdown());
}

// An exporter that does nothing but record (and give back ) the # of times Shutdown was called.
class RecordShutdownExporter final : public SpanExporter
{
public:
  RecordShutdownExporter(int *shutdown_counter) : shutdown_counter_(shutdown_counter) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData());
  }

  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    return ExportResult::kSuccess;
  }

  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    *shutdown_counter_ += 1;
    return true;
  }

private:
  int *shutdown_counter_;
};

TEST(SimpleSpanProcessor, ShutdownCalledOnce)
{
  int shutdowns = 0;
  std::unique_ptr<RecordShutdownExporter> exporter(new RecordShutdownExporter(&shutdowns));
  SimpleSpanProcessor processor(std::move(exporter));
  EXPECT_EQ(0, shutdowns);
  processor.Shutdown();
  EXPECT_EQ(1, shutdowns);
  processor.Shutdown();
  EXPECT_EQ(1, shutdowns);
}
