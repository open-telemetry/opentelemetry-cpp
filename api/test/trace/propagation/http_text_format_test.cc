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

static trace::propagation::HttpTraceContext<std::map<std::string, std::string>> format =
    trace::propagation::HttpTraceContext<std::map<std::string, std::string>>();

using MapHttpTraceContext =
    trace::propagation::HttpTraceContext<std::map<std::string, std::string>>;
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

TEST(HTTPTextFormatTest, HeadersWithTraceState)
{
  const std::map<std::string, std::string> carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},
      {"tracestate", "congo=congosSecondPosition,rojo=rojosFirstPosition"}};
  context::Context ctx1 =
      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
  context::Context ctx2                 = format.Extract(Getter, carrier, ctx1);
//  std::map<std::string, std::string> c2 = {};
//  format.Inject(Setter, c2, ctx2);
//  EXPECT_EQ(c2["traceparent"], "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01");
//  EXPECT_EQ(c2["tracestate"], "congo=congosSecondPosition,rojo=rojosFirstPosition");
//  EXPECT_EQ(carrier.size(), c2.size());
}

//TEST(HTTPTextFormatTest, NoTraceParentHeader)
//{
//  // When trace context headers are not present, a new SpanContext
//  // should be created.
//  const std::map<std::string, std::string> carrier = {};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span     = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  EXPECT_EQ(span->GetContext().trace_id(), trace::SpanContext(false, false).trace_id());
//  EXPECT_EQ(span->GetContext().span_id(), trace::SpanContext(false, false).span_id());
//  EXPECT_EQ(span->GetContext().trace_flags(), trace::SpanContext(false, false).trace_flags());
//  EXPECT_EQ(span->GetContext().trace_state(), trace::SpanContext(false, false).trace_state());
//}
//
//TEST(HTTPTextFormatTest, InvalidTraceId)
//{
//  // If the trace id is invalid, we must ignore the full trace parent header,
//  // and return a random, valid trace.
//  // Also ignore any trace state.
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-00000000000000000000000000000000-1234567890123456-00"},
//      {"tracestate", "foo=1,bar=2,foo=3"}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span     = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  EXPECT_EQ(span->GetContext().trace_id(), trace::SpanContext(false, false).trace_id());
//  EXPECT_EQ(span->GetContext().span_id(), trace::SpanContext(false, false).span_id());
//  EXPECT_EQ(span->GetContext().trace_flags(), trace::SpanContext(false, false).trace_flags());
//  EXPECT_EQ(span->GetContext().trace_state(), trace::SpanContext(false, false).trace_state());
//}
//
//TEST(HTTPTextFormatTest, InvalidParentId)
//{
//  // If the parent id is invalid, we must ignore the full trace parent
//  // header.
//  // Also ignore any trace state.
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-00000000000000000000000000000000-0000000000000000-00"},
//      {"tracestate", "foo=1,bar=2,foo=3"}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span     = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  EXPECT_EQ(span->GetContext().trace_id(), trace::SpanContext(false, false).trace_id());
//  EXPECT_EQ(span->GetContext().span_id(), trace::SpanContext(false, false).span_id());
//  EXPECT_EQ(span->GetContext().trace_flags(), trace::SpanContext(false, false).trace_flags());
//  EXPECT_EQ(span->GetContext().trace_state(), trace::SpanContext(false, false).trace_state());
//}
//
//TEST(HTTPTextFormatTest, NoSendEmptyTraceState)
//{
//  // If the trace state is empty, do not set the header.
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2                 = format.Extract(Getter, carrier, ctx1);
//  std::map<std::string, std::string> c2 = {};
//  format.Inject(Setter, c2, ctx2);
//  EXPECT_TRUE(carrier.count("traceparent") > 0);
//  EXPECT_FALSE(carrier.count("tracestate") > 0);
//}
//
//TEST(HTTPTextFormatTest, FormatNotSupported)
//{
//  // If the trace parent does not adhere to the supported format, discard it and
//  // create a new trace context.
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00-residue"},
//      {"tracestate", "foo=1,bar=2,foo=3"}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span     = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  EXPECT_FALSE(span->GetContext().IsValid());
//  EXPECT_EQ(span->GetContext().trace_id(), trace::SpanContext(false, false).trace_id());
//  EXPECT_EQ(span->GetContext().span_id(), trace::SpanContext(false, false).span_id());
//  EXPECT_EQ(span->GetContext().trace_flags(), trace::SpanContext(false, false).trace_flags());
//  EXPECT_EQ(span->GetContext().trace_state(), trace::SpanContext(false, false).trace_state());
//}
//
//TEST(HTTPTextFormatTest, PropagateInvalidContext)
//{
//  // Do not propagate invalid trace context.
//  std::map<std::string, std::string> carrier = {};
//  context::Context ctx{
//      "current-span",
//      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
//  format.Inject(Setter, carrier, ctx);
//  EXPECT_TRUE(carrier.count("traceparent") == 0);
//}
//
//TEST(HTTPTextFormatTest, TraceStateHeaderWithTrailingComma)
//{
//  // Do not propagate invalid trace context.
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00"},
//      {"tracestate", "foo=1,"}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2         = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span             = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  trace::TraceState trace_state = span->GetContext().trace_state();
//  nostd::string_view value;
//  EXPECT_TRUE(trace_state.Get("foo", value));
//  EXPECT_EQ(value, "1");
//}
//
//TEST(HTTPTextFormatTest, TraceStateKeys)
//{
//  // Test for valid key patterns in the tracestate
//  std::string trace_state_value = "1a-2f@foo=bar1,1a-_*/2b@foo=bar2,foo=bar3,foo-_*/bar=bar4";
//  const std::map<std::string, std::string> carrier = {
//      {"traceparent", "00-12345678901234567890123456789012-1234567890123456-00"},
//      {"tracestate", trace_state_value}};
//  context::Context ctx1 =
//      context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
//  context::Context ctx2         = format.Extract(Getter, carrier, ctx1);
//  trace::Span *span             = MapHttpTraceContext::GetCurrentSpan(ctx2);
//  trace::TraceState trace_state = span->GetContext().trace_state();
//  nostd::string_view value;
//  EXPECT_TRUE(trace_state.Get("1a-2f@foo", value));
//  EXPECT_EQ(value, "bar1");
//  EXPECT_TRUE(trace_state.Get("1a-_*/2b@foo", value));
//  EXPECT_EQ(value, "bar2");
//  EXPECT_TRUE(trace_state.Get("foo", value));
//  EXPECT_EQ(value, "bar3");
//  EXPECT_TRUE(trace_state.Get("foo-_*/bar", value));
//  EXPECT_EQ(value, "bar4");
//}
