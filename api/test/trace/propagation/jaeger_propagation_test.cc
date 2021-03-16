#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/trace/scope.h"
#include "util.h"

#include <map>

#include <gtest/gtest.h>

using namespace opentelemetry;

static nostd::string_view Getter(const std::map<std::string, std::string> &carrier,
                                 nostd::string_view trace_type = "uber-trace-id")
{
  auto it = carrier.find(std::string(trace_type));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(std::map<std::string, std::string> &carrier,
                   nostd::string_view trace_type        = "uber-trace-id",
                   nostd::string_view trace_description = "")
{
  carrier[std::string(trace_type)] = std::string(trace_description);
}

using Propagator = trace::propagation::JaegerPropagator<std::map<std::string, std::string>>;

static Propagator format = Propagator();

TEST(JaegerPropagatorTest, ExtractValidSpans)
{
  struct TestTrace
  {
    std::string trace_state;
    std::string expected_trace_id;
    std::string expected_span_id;
    bool sampled;
  };

  std::vector<TestTrace> traces = {
      {
          "4bf92f3577b34da6a3ce929d0e0e4736:0102030405060708:0:00",
          "4bf92f3577b34da6a3ce929d0e0e4736",
          "0102030405060708",
          false,
      },
      {
          "4bf92f3577b34da6a3ce929d0e0e4736:0102030405060708:0:ff",
          "4bf92f3577b34da6a3ce929d0e0e4736",
          "0102030405060708",
          true,
      },
      {
          "4bf92f3577b34da6a3ce929d0e0e4736:0102030405060708:0:f",
          "4bf92f3577b34da6a3ce929d0e0e4736",
          "0102030405060708",
          true,
      },
      {
          "a3ce929d0e0e4736:0102030405060708:0:00",
          "0000000000000000a3ce929d0e0e4736",
          "0102030405060708",
          false,
      },
      {
          "A3CE929D0E0E4736:ABCDEFABCDEF1234:0:01",
          "0000000000000000a3ce929d0e0e4736",
          "abcdefabcdef1234",
          true,
      },
      {
          "ff:ABCDEFABCDEF1234:0:0",
          "000000000000000000000000000000ff",
          "abcdefabcdef1234",
          false,
      },
      {
          "4bf92f3577b34da6a3ce929d0e0e4736:0102030405060708:0102030405060708:00",
          "4bf92f3577b34da6a3ce929d0e0e4736",
          "0102030405060708",
          false,
      },

  };

  for (TestTrace &test_trace : traces)
  {
    const std::map<std::string, std::string> carrier = {{"uber-trace-id", test_trace.trace_state}};
    context::Context ctx1                            = context::Context{};
    context::Context ctx2                            = format.Extract(Getter, carrier, ctx1);

    auto span = trace::propagation::detail::GetCurrentSpan(ctx2);
    EXPECT_TRUE(span.IsValid());

    EXPECT_EQ(Hex(span.trace_id()), test_trace.expected_trace_id);
    EXPECT_EQ(Hex(span.span_id()), test_trace.expected_span_id);
    EXPECT_EQ(span.IsSampled(), test_trace.sampled);
    EXPECT_EQ(span.IsRemote(), true);
  }
}

TEST(JaegerPropagatorTest, ExctractInvalidSpans)
{
  std::vector<std::string> traces = {
      "4bf92f3577b34da6a3ce929d0e0e47344:0102030405060708:0:00",  // too long trace id
      "4bf92f3577b34da6a3ce929d0e0e4734:01020304050607089:0:00",  // too long span id
      "4bf92f3577b34da6x3ce929d0y0e4734:01020304050607089:0:00",  // invalid trace id character
      "4bf92f3577b34da6a3ce929d0e0e4734:01020304g50607089:0:00",  // invalid span id character
      "4bf92f3577b34da6a3ce929d0e0e4734::0:00",
      "",
      "::::",
      "0:0:0:0",
      ":abcdef12:0:0",
  };

  for (auto &trace : traces)
  {
    const std::map<std::string, std::string> carrier = {{"uber-trace-id", trace}};
    context::Context ctx1                            = context::Context{};
    context::Context ctx2                            = format.Extract(Getter, carrier, ctx1);

    auto span = trace::propagation::detail::GetCurrentSpan(ctx2);
    EXPECT_FALSE(span.IsValid());
  }
}

TEST(JaegerPropagatorTest, InjectsContext)
{
  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  trace::SpanContext span_context{trace::TraceId{buf_trace}, trace::SpanId{buf_span},
                                  trace::TraceFlags{true}, false};
  nostd::shared_ptr<trace::Span> sp{new trace::DefaultSpan{span_context}};
  trace::Scope scoped_span{sp};

  std::map<std::string, std::string> headers = {};
  format.Inject(Setter, headers, context::RuntimeContext::GetCurrent());
  EXPECT_EQ(headers["uber-trace-id"], "0102030405060708090a0b0c0d0e0f10:0102030405060708:0:01");
}

TEST(JaegerPropagatorTest, DoNotInjectInvalidContext)
{
  std::map<std::string, std::string> carrier = {};
  context::Context ctx{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  format.Inject(Setter, carrier, ctx);
  EXPECT_TRUE(carrier.count("uber-trace-id") == 0);
}
