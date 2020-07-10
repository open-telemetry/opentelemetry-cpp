#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/recordable.h"

#include "opentelemetry/sdk/trace/exporter.h"

#include "exporters/trace/stdout/stdout.h"


#include <iostream>

#include <gtest/gtest.h>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

TEST(StdoutSpanExporter, Shutdown)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();
  recordable->SetName("Test Span");

  // This can be an ofstream as well or any other ostream
  std::stringstream buffer;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer or any other ostream
  std::cout.rdbuf(buffer.rdbuf());

  processor->Shutdown();
  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  ASSERT_EQ(buffer.str(),"");
}

TEST(StdoutSpanExporter, PrintDefaultSpan)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();

  // This can be an ofstream as well or any other ostream
  std::stringstream buffer;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer or any other ostream
  std::cout.rdbuf(buffer.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  const char* expectedOutput =
 "{\n"
  "  name          : \n"
  "  trace_id      : 00000000000000000000000000000000\n"
  "  span_id       : 0000000000000000\n"
  "  parent_span_id: 0000000000000000\n"
  "  start         : 0\n"
  "  duration      : 0\n"
  "  description   : \n"
  "  status        : OK\n"
  "  attributes    : \n"
  "}\n"; 
  ASSERT_EQ(buffer.str(),expectedOutput);
}

TEST(StdoutSpanExporter, PrintChangedSpan)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();

  recordable->SetName("Test Span");
  opentelemetry::core::SystemTimestamp now(std::chrono::system_clock::now());

  recordable->SetStartTime(now);
  recordable->SetDuration(std::chrono::nanoseconds(100));
  recordable->SetStatus(opentelemetry::trace::CanonicalCode::UNIMPLEMENTED,"Test Description");

  recordable->SetAttribute("attr1", 314159);
  recordable->SetAttribute("attr2", false);
  recordable->SetAttribute("attr3", "string");

  // This can be an ofstream as well or any other ostream
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer or any other ostream
  std::cout.rdbuf(stdoutOutput.rdbuf());
  processor->OnEnd(std::move(recordable));
  std::cout.rdbuf(sbuf);

  std::string start = std::to_string(now.time_since_epoch().count());
  std::string duration = std::to_string(100);
  // optionaly:

  std::string expectedOutput =
 "{\n"
  "  name          : Test Span\n"
  "  trace_id      : 00000000000000000000000000000000\n"
  "  span_id       : 0000000000000000\n"
  "  parent_span_id: 0000000000000000\n"
  "  start         : " + start + "\n"
  "  duration      : " + duration + "\n"
  "  description   : Test Description\n"
  "  status        : UNIMPLEMENTED\n"
  "  attributes    : attr1: 314159 attr3: \"string\" attr2: false \n"
  "}\n"; 
  ASSERT_EQ(stdoutOutput.str(),expectedOutput);
}

