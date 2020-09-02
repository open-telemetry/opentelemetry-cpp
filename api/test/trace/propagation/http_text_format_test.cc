#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/http_text_format.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"

using namespace opentelemetry;

static nostd::string_view Getter(const std::map<std::string, std::string> &carrier,
                                 nostd::string_view trace_type = "traceparent")
{
  auto it = carrier.find(std::string(trace_type));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(std::map<std::string, std::string> &carrier,
                   nostd::string_view trace_type        = "traceparent",
                   nostd::string_view trace_description = "")
{
  carrier[std::string(trace_type)] = std::string(trace_description);
}

using MapHttpTraceContext =
    trace::propagation::HttpTraceContext<std::map<std::string, std::string>>;

static MapHttpTraceContext format = MapHttpTraceContext();

TEST(HTTPTextFormatTest, TraceIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapHttpTraceContext::GenerateTraceIdFromString("01020304050607080807aabbccddeeff"),
            trace::TraceId(buf));
}

TEST(HTTPTextFormatTest, SpanIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapHttpTraceContext::GenerateSpanIdFromString("0102aabbccddeeff"), trace::SpanId(buf));
}

TEST(HTTPTextFormatTest, TraceFlagsBufferGeneration)
{
  EXPECT_EQ(MapHttpTraceContext::GenerateTraceFlagsFromString("00"), trace::TraceFlags());
}

TEST(HTTPTextFormatTest, NoSendEmptyTraceState)
{
  // If the trace state is empty, do not set the header.
  const std::map<std::string, std::string> carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"}};
  context::Context ctx1 =
      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
  context::Context ctx2                 = format.Extract(Getter, carrier, ctx1);
  std::map<std::string, std::string> c2 = {};
  format.Inject(Setter, c2, ctx2);
  EXPECT_TRUE(carrier.count("traceparent") > 0);
  EXPECT_FALSE(carrier.count("tracestate") > 0);
}

TEST(HTTPTextFormatTest, PropagateInvalidContext)
{
  // Do not propagate invalid trace context.
  std::map<std::string, std::string> carrier = {};
  context::Context ctx{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  format.Inject(Setter, carrier, ctx);
  EXPECT_TRUE(carrier.count("traceparent") == 0);
}
