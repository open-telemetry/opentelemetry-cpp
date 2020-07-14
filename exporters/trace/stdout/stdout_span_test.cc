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

// Testing Shutdown functionality of StdoutSpanExporter, should expect no data to be sent to stdout
TEST(StdoutSpanExporter, Shutdown)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();
  recordable->SetName("Test Span");

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  processor->Shutdown();
  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  ASSERT_EQ(stdoutOutput.str(),"");
}

// Testing what a default span that is not changed will print out, either all 0's or empty values
TEST(StdoutSpanExporter, PrintDefaultSpan)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
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
  ASSERT_EQ(stdoutOutput.str(),expectedOutput);
}

// Testing if the changes we make to a span will carry over through the exporter
TEST(StdoutSpanExporter, PrintChangedSpanCout)
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

  std::array<int, 3> array1 = {1, 2, 3};
  opentelemetry::nostd::span<int> span1{array1.data(), array1.size()};  
  recordable->SetAttribute("attr4", span1);

  std::array<bool, 3> array2 = {false, true, false};
  opentelemetry::nostd::span<bool> span2{array2.data(), array2.size()};  
  recordable->SetAttribute("attr5", span2);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  std::string start = std::to_string(now.time_since_epoch().count());

  std::string expectedOutput =
 "{\n"
  "  name          : Test Span\n"
  "  trace_id      : 00000000000000000000000000000000\n"
  "  span_id       : 0000000000000000\n"
  "  parent_span_id: 0000000000000000\n"
  "  start         : " + start + "\n"
  "  duration      : 100\n"
  "  description   : Test Description\n"
  "  status        : UNIMPLEMENTED\n"
  "  attributes    : attr1: 314159 attr3: string attr5: [0,1,0] attr2: 0 attr4: [1,2,3] \n"
  "}\n"; 
  ASSERT_EQ(stdoutOutput.str(),expectedOutput);
}

// PrintChangedSpan to std::cerr
TEST(StdoutSpanExporter, PrintChangedSpanCerr)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter(std::cerr));
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

  std::array<int, 3> array1 = {1, 2, 3};
  opentelemetry::nostd::span<int> span1{array1.data(), array1.size()};  
  recordable->SetAttribute("attr4", span1);

  std::array<bool, 3> array2 = {false, true, false};
  opentelemetry::nostd::span<bool> span2{array2.data(), array2.size()};  
  recordable->SetAttribute("attr5", span2);

  // Create stringstream to redirect to
  std::stringstream stdcerrOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cerr.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cerr.rdbuf(stdcerrOutput.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::cerr.rdbuf(sbuf);

  std::string start = std::to_string(now.time_since_epoch().count());

  std::string expectedOutput =
 "{\n"
  "  name          : Test Span\n"
  "  trace_id      : 00000000000000000000000000000000\n"
  "  span_id       : 0000000000000000\n"
  "  parent_span_id: 0000000000000000\n"
  "  start         : " + start + "\n"
  "  duration      : 100\n"
  "  description   : Test Description\n"
  "  status        : UNIMPLEMENTED\n"
  "  attributes    : attr1: 314159 attr3: string attr5: [0,1,0] attr2: 0 attr4: [1,2,3] \n"
  "}\n"; 
  ASSERT_EQ(stdcerrOutput.str(),expectedOutput);
}

// PrintChangedSpan to std::clog
TEST(StdoutSpanExporter, PrintChangedSpanClog)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new opentelemetry::exporter::trace::StdoutSpanExporter(std::clog));
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

  std::array<int, 3> array1 = {1, 2, 3};
  opentelemetry::nostd::span<int> span1{array1.data(), array1.size()};  
  recordable->SetAttribute("attr4", span1);

  std::array<bool, 3> array2 = {false, true, false};
  opentelemetry::nostd::span<bool> span2{array2.data(), array2.size()};  
  recordable->SetAttribute("attr5", span2);

  // Create stringstream to redirect to
  std::stringstream stdclogOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::clog.rdbuf();

  // Redirect cout to our stringstream buffer
  std::clog.rdbuf(stdclogOutput.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::clog.rdbuf(sbuf);

  std::string start = std::to_string(now.time_since_epoch().count());

  std::string expectedOutput =
 "{\n"
  "  name          : Test Span\n"
  "  trace_id      : 00000000000000000000000000000000\n"
  "  span_id       : 0000000000000000\n"
  "  parent_span_id: 0000000000000000\n"
  "  start         : " + start + "\n"
  "  duration      : 100\n"
  "  description   : Test Description\n"
  "  status        : UNIMPLEMENTED\n"
  "  attributes    : attr1: 314159 attr3: string attr5: [0,1,0] attr2: 0 attr4: [1,2,3] \n"
  "}\n"; 
  ASSERT_EQ(stdclogOutput.str(),expectedOutput);
}
