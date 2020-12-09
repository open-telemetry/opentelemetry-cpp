/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/sdk/logs/logger.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;
namespace sdktrace = opentelemetry::sdk::trace;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::log() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnReceive()

  auto lp     = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());
  auto logger = lp->GetLogger("logger");

  // Log a sample log record to a nullptr processor
  opentelemetry::logs::LogRecord r;
  r.name = "Test log";
  logger->log(r);
}

// Define a global log record that will be modified when the Log() method is called
static opentelemetry::nostd::shared_ptr<opentelemetry::logs::LogRecord> record_;

class DummyProcessor : public LogProcessor
{
  void OnReceive(std::shared_ptr<opentelemetry::logs::LogRecord> record) noexcept
  {
    record_ = opentelemetry::nostd::shared_ptr<opentelemetry::logs::LogRecord>(
        new opentelemetry::logs::LogRecord(*record.get()));
  }
  bool ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
};

TEST(LoggerSDK, LogToAProcessor)
{
  // Create an API LoggerProvider and logger
  auto api_lp = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());
  auto logger = api_lp->GetLogger("logger");

  // Cast the API LoggerProvider to an SDK Logger Provider and assert that it is still the same
  // LoggerProvider by checking that getting a logger with the same name as the previously defined
  // logger is the same instance
  auto lp      = static_cast<LoggerProvider *>(api_lp.get());
  auto logger2 = lp->GetLogger("logger");
  ASSERT_EQ(logger, logger2);

  // Set a processor for the LoggerProvider
  std::shared_ptr<LogProcessor> processor = std::shared_ptr<LogProcessor>(new DummyProcessor());
  lp->SetProcessor(processor);
  ASSERT_EQ(processor, lp->GetProcessor());

  // Should later introduce a way to assert that
  // the logger's processor is the same as "proc"
  // and that the logger's processor is the same as lp's processor

  // Log a sample log record to the processor
  opentelemetry::logs::LogRecord r;
  r.name = "Test log";
  logger->log(r);
}

TEST(LoggerSDK, DefaultValueInjection)
{
  // Use the DummyProcessor defined above, which saves the lastest record in the
  // _record global variable
  std::shared_ptr<LogProcessor> processor = std::shared_ptr<LogProcessor>(new DummyProcessor());
  auto lp                                 = std::shared_ptr<LoggerProvider>(new LoggerProvider());
  lp->SetProcessor(processor);
  auto logger = lp->GetLogger("Logger1");

  // Log a sample log record to the processor
  opentelemetry::logs::LogRecord r;
  r.name = "Test log";
  logger->log(r);

  // Check that the log record has injected values
  // Timestamp shouldn't equal 0
  ASSERT_NE(record_->timestamp, opentelemetry::core::SystemTimestamp(std::chrono::seconds(0)));

  // Check that the traceid, spanid, and traceflags are not valid since there is no trace context
  ASSERT_FALSE(record_->trace_id.IsValid());
  ASSERT_FALSE(record_->span_id.IsValid());
  ASSERT_FALSE(record_->trace_flags.IsSampled());

  // To test traceid/spanid/traceflags injection, initialize the tracing pipeline
  std::unique_ptr<opentelemetry::exporter::memory::InMemorySpanExporter> trace_exporter(
      new opentelemetry::exporter::memory::InMemorySpanExporter());
  auto trace_processor = std::make_shared<sdktrace::SimpleSpanProcessor>(std::move(trace_exporter));
  auto trace_provider  = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(trace_processor));
  opentelemetry::trace::Provider::SetTracerProvider(trace_provider);

  // Create a tracer and start a span for span context
  auto tracer      = trace_provider->GetTracer("foo_library");
  auto span_first  = tracer->StartSpan("span 1");
  auto scope_first = tracer->WithActiveSpan(span_first);
  auto span_second = tracer->StartSpan("span 2");

  // Log a sample log record to the processor
  opentelemetry::logs::LogRecord r2;
  r2.name = "Test log";
  logger->log(r2);

  // Check that the traceid, spanid, and traceflags were injected from the span context properly
  auto span_context = tracer->GetCurrentSpan()->GetContext();
  ASSERT_EQ(record_->trace_id, span_context.trace_id());
  ASSERT_EQ(record_->span_id, span_context.span_id());
  ASSERT_EQ(record_->trace_flags, span_context.trace_flags());
}