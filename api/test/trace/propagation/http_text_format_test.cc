#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"

using namespace opentelemetry;

template <typename T>
static std::string Hex(const T &id_item)
{
  char buf[T::kSize * 2];
  id_item.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}

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

TEST(TextMapPropagatorTest, TraceIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapHttpTraceContext::GenerateTraceIdFromString("01020304050607080807aabbccddeeff"),
            trace::TraceId(buf));
}

TEST(TextMapPropagatorTest, SpanIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapHttpTraceContext::GenerateSpanIdFromString("0102aabbccddeeff"), trace::SpanId(buf));
}

TEST(TextMapPropagatorTest, TraceFlagsBufferGeneration)
{
  EXPECT_EQ(MapHttpTraceContext::GenerateTraceFlagsFromString("00"), trace::TraceFlags());
}

TEST(TextMapPropagatorTest, NoSendEmptyTraceState)
{
  // If the trace state is empty, do not set the header.
  const std::map<std::string, std::string> carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"}};
  context::Context ctx1 = context::Context{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  context::Context ctx2                 = format.Extract(Getter, carrier, ctx1);
  std::map<std::string, std::string> c2 = {};
  format.Inject(Setter, c2, ctx2);
  EXPECT_TRUE(carrier.count("traceparent") > 0);
  EXPECT_FALSE(carrier.count("tracestate") > 0);
}

TEST(TextMapPropagatorTest, PropagateInvalidContext)
{
  // Do not propagate invalid trace context.
  std::map<std::string, std::string> carrier = {};
  context::Context ctx{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  format.Inject(Setter, carrier, ctx);
  EXPECT_TRUE(carrier.count("traceparent") == 0);
}

TEST(TextMapPropagatorTest, SetRemoteSpan)
{
  const std::map<std::string, std::string> carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));

  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  EXPECT_EQ(Hex(span->GetContext().trace_id()), "4bf92f3577b34da6a3ce929d0e0e4736");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "0102030405060708");
  EXPECT_EQ(span->GetContext().IsSampled(), true);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST(TextMapPropagatorTest, GetCurrentSpan)
{
  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  trace::SpanContext span_context{trace::TraceId{buf_trace}, trace::SpanId{buf_span},
                                  trace::TraceFlags{true}, false};
  nostd::shared_ptr<trace::Span> sp{new trace::DefaultSpan{span_context}};

  // Set `sp` as the currently active span, which must be used by `Inject`.
  trace::Scope scoped_span{sp};

  std::map<std::string, std::string> headers = {};
  format.Inject(Setter, headers, context::RuntimeContext::GetCurrent());
  EXPECT_EQ(headers["traceparent"], "00-0102030405060708090a0b0c0d0e0f10-0102030405060708-01");
}
