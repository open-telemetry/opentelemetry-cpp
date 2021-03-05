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
#include "opentelemetry/trace/propagation/b3_propagator.h"
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
                                 nostd::string_view key)
{
  auto it = carrier.find(std::string(key));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(std::map<std::string, std::string> &carrier,
                   nostd::string_view key,
                   nostd::string_view value = "")
{
  carrier[std::string(key)] = std::string(value);
}

using MapB3Context = trace::propagation::B3Propagator<std::map<std::string, std::string>>;

static MapB3Context format = MapB3Context();

using MapB3ContextMultiHeader =
    trace::propagation::B3PropagatorMultiHeader<std::map<std::string, std::string>>;

static MapB3ContextMultiHeader formatMultiHeader = MapB3ContextMultiHeader();

TEST(B3PropagationTest, TraceIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapB3Context::GenerateTraceIdFromString("01020304050607080807aabbccddeeff"),
            trace::TraceId(buf));
}

TEST(B3PropagationTest, SpanIdBufferGeneration)
{
  constexpr uint8_t buf[] = {1, 2, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(MapB3Context::GenerateSpanIdFromString("0102aabbccddeeff"), trace::SpanId(buf));
}

TEST(B3PropagationTest, TraceFlagsBufferGeneration)
{
  EXPECT_EQ(MapB3Context::GenerateTraceFlagsFromString("0"), trace::TraceFlags());
  EXPECT_EQ(MapB3Context::GenerateTraceFlagsFromString("1"),
            trace::TraceFlags(trace::TraceFlags::kIsSampled));
}

TEST(B3PropagationTest, PropagateInvalidContext)
{
  // Do not propagate invalid trace context.
  std::map<std::string, std::string> carrier = {};
  context::Context ctx{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  format.Inject(Setter, carrier, ctx);
  EXPECT_TRUE(carrier.count("b3") == 0);
}

TEST(B3PropagationTest, ExtractInvalidContext)
{
  const std::map<std::string, std::string> carrier = {
      {"b3", "00000000000000000000000000000000-0000000000000000-0"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
  auto ctx2_span        = ctx2.GetValue(trace::kSpanKey);
  auto span             = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);
  EXPECT_EQ(span->GetContext().IsRemote(), false);
}

TEST(B3PropagationTest, DoNotExtractWithInvalidHex)
{
  const std::map<std::string, std::string> carrier = {
      {"b3", "0000000zzz0000000000000000000000-0000000zzz000000-1"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
  auto ctx2_span        = ctx2.GetValue(trace::kSpanKey);
  auto span             = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);
  EXPECT_EQ(span->GetContext().IsRemote(), false);
}

TEST(B3PropagationTest, SetRemoteSpan)
{
  const std::map<std::string, std::string> carrier = {
      {"b3", "80f198ee56343ba864fe8b2a57d3eff7-e457b5a2e4d86bd1-1-05e3ac9a4f6e3b90"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));

  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  EXPECT_EQ(Hex(span->GetContext().trace_id()), "80f198ee56343ba864fe8b2a57d3eff7");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "e457b5a2e4d86bd1");
  EXPECT_EQ(span->GetContext().IsSampled(), true);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST(B3PropagationTest, SetRemoteSpan_TraceIdShort)
{
  const std::map<std::string, std::string> carrier = {
      {"b3", "80f198ee56343ba8-e457b5a2e4d86bd1-1-05e3ac9a4f6e3b90"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));

  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  EXPECT_EQ(Hex(span->GetContext().trace_id()), "000000000000000080f198ee56343ba8");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "e457b5a2e4d86bd1");
  EXPECT_EQ(span->GetContext().IsSampled(), true);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST(B3PropagationTest, SetRemoteSpan_SingleHeaderNoFlags)
{
  const std::map<std::string, std::string> carrier = {
      {"b3", "80f198ee56343ba864fe8b2a57d3eff7-e457b5a2e4d86bd1"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));

  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  EXPECT_EQ(Hex(span->GetContext().trace_id()), "80f198ee56343ba864fe8b2a57d3eff7");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "e457b5a2e4d86bd1");
  EXPECT_EQ(span->GetContext().IsSampled(), false);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST(B3PropagationTest, SetRemoteSpanMultiHeader)
{
  const std::map<std::string, std::string> carrier = {
      {"X-B3-TraceId", "80f198ee56343ba864fe8b2a57d3eff7"},
      {"X-B3-SpanId", "e457b5a2e4d86bd1"},
      {"X-B3-Sampled", "1"}};
  context::Context ctx1 = context::Context{};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));

  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  EXPECT_EQ(Hex(span->GetContext().trace_id()), "80f198ee56343ba864fe8b2a57d3eff7");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "e457b5a2e4d86bd1");
  EXPECT_EQ(span->GetContext().IsSampled(), true);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST(B3PropagationTest, GetCurrentSpan)
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
  EXPECT_EQ(headers["b3"], "0102030405060708090a0b0c0d0e0f10-0102030405060708-1");
}

TEST(B3PropagationTest, GetCurrentSpanMultiHeader)
{
  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  trace::SpanContext span_context{trace::TraceId{buf_trace}, trace::SpanId{buf_span},
                                  trace::TraceFlags{true}, false};
  nostd::shared_ptr<trace::Span> sp{new trace::DefaultSpan{span_context}};

  // Set `sp` as the currently active span, which must be used by `Inject`.
  trace::Scope scoped_span{sp};

  std::map<std::string, std::string> headers = {};
  formatMultiHeader.Inject(Setter, headers, context::RuntimeContext::GetCurrent());
  EXPECT_EQ(headers["X-B3-TraceId"], "0102030405060708090a0b0c0d0e0f10");
  EXPECT_EQ(headers["X-B3-SpanId"], "0102030405060708");
  EXPECT_EQ(headers["X-B3-Sampled"], "1");
}
