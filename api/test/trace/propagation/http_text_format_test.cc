#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/trace/propagation/global_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/scope.h"
#include "util.h"

#include <map>
#include <unordered_map>

#include <gtest/gtest.h>

using namespace opentelemetry;

using MapStr          = std::map<std::string, std::string>;
using UnorderedMapStr = std::unordered_map<std::string, std::string>;

static nostd::string_view Getter(const MapStr &carrier,
                                 nostd::string_view trace_type = "traceparent")
{
  auto it = carrier.find(std::string(trace_type));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(MapStr &carrier,
                   nostd::string_view trace_type        = "traceparent",
                   nostd::string_view trace_description = "")
{
  carrier[std::string(trace_type)] = std::string(trace_description);
}

using MapHttpTraceContext = trace::propagation::HttpTraceContext<MapStr>;

static MapHttpTraceContext format = MapHttpTraceContext();

TEST(TextMapPropagatorTest, TraceFlagsBufferGeneration)
{
  EXPECT_EQ(MapHttpTraceContext::TraceFlagsFromHex("00"), trace::TraceFlags());
}

TEST(TextMapPropagatorTest, NoSendEmptyTraceState)
{
  // If the trace state is empty, do not set the header.
  const MapStr carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"}};
  context::Context ctx1 = context::Context{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
  MapStr c2             = {};
  format.Inject(Setter, c2, ctx2);
  EXPECT_TRUE(carrier.count("traceparent") > 0);
  EXPECT_FALSE(carrier.count("tracestate") > 0);
}

TEST(TextMapPropagatorTest, PropogateTraceState)
{
  const MapStr carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},
      {"tracestate", "congo=t61rcWkgMzE"}};
  context::Context ctx1 = context::Context{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  context::Context ctx2 = format.Extract(Getter, carrier, ctx1);
  MapStr c2             = {};
  format.Inject(Setter, c2, ctx2);

  EXPECT_TRUE(carrier.count("traceparent") > 0);
  EXPECT_TRUE(carrier.count("tracestate") > 0);
  EXPECT_EQ(c2["tracestate"], "congo=t61rcWkgMzE");
}

TEST(TextMapPropagatorTest, PropagateInvalidContext)
{
  // Do not propagate invalid trace context.
  MapStr carrier = {};
  context::Context ctx{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  format.Inject(Setter, carrier, ctx);
  EXPECT_TRUE(carrier.count("traceparent") == 0);
  EXPECT_TRUE(carrier.count("tracestate") == 0);
}

TEST(TextMapPropagatorTest, SetRemoteSpan)
{
  const MapStr carrier = {
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

  auto trace_state = trace::TraceState::FromHeader("congo=t61rcWkgMzE");
  trace::SpanContext span_context{trace::TraceId{buf_trace}, trace::SpanId{buf_span},
                                  trace::TraceFlags{true}, false, trace_state};
  nostd::shared_ptr<trace::Span> sp{new trace::DefaultSpan{span_context}};

  // Set `sp` as the currently active span, which must be used by `Inject`.
  trace::Scope scoped_span{sp};

  MapStr headers = {};
  format.Inject(Setter, headers, context::RuntimeContext::GetCurrent());
  EXPECT_EQ(headers["traceparent"], "00-0102030405060708090a0b0c0d0e0f10-0102030405060708-01");
  EXPECT_EQ(headers["tracestate"], "congo=t61rcWkgMzE");
}

TEST(TextMapPropagatorTest, InvalidIdentitiesAreNotExtracted)
{
  std::vector<std::string> traces = {
      "ff-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01",
      "00-0af7651916cd43dd8448eb211c80319c1-b9c7c989f97918e1-01",
      "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e11-01",
      "0-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-01",
      "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97918e1-0",
      "00-0af7651916cd43dd8448eb211c8031-b9c7c989f97918e1-01",
      "00-0af7651916cd43dd8448eb211c80319c-b9c7c989f97-01",
      "00-1-1-00",
      "00--b9c7c989f97918e1-01",
      "00-0af7651916cd43dd8448eb211c80319c1--01",
      "",
      "---",
  };

  for (auto &trace : traces)
  {
    MapStr carrier        = {{"traceparent", trace}};
    context::Context ctx1 = context::Context{};
    context::Context ctx2 = format.Extract(Getter, carrier, ctx1);

    auto span = trace::propagation::detail::GetCurrentSpan(ctx2);
    EXPECT_FALSE(span.IsValid());
  }
}

TEST(GlobalPropagator, SetAndGet)
{

  trace::propagation::GlobalPropagator<MapStr>::SetGlobalPropagator(
      nostd::shared_ptr<trace::propagation::TextMapPropagator<MapStr>>(new MapHttpTraceContext()));

  auto propagator = trace::propagation::GlobalPropagator<MapStr>::GetGlobalPropagator();

  const MapStr carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},
      {"tracestate", "congo=t61rcWkgMzE"}};
  context::Context ctx1 = context::Context{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  context::Context ctx2 = propagator->Extract(Getter, carrier, ctx1);
  MapStr c2             = {};
  propagator->Inject(Setter, c2, ctx2);

  EXPECT_TRUE(carrier.count("traceparent") > 0);
  EXPECT_TRUE(carrier.count("tracestate") > 0);
  EXPECT_EQ(c2["tracestate"], "congo=t61rcWkgMzE");
}

using UnorderedMapStr = std::unordered_map<std::string, std::string>;

static nostd::string_view Getter(const UnorderedMapStr &carrier,
                                 nostd::string_view trace_type = "traceparent")
{
  auto it = carrier.find(std::string(trace_type));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(UnorderedMapStr &carrier,
                   nostd::string_view trace_type        = "traceparent",
                   nostd::string_view trace_description = "")
{
  carrier[std::string(trace_type)] = std::string(trace_description);
}

TEST(GlobalPropagator, NoOpPropagator)
{

  auto propagator = trace::propagation::GlobalPropagator<
      UnorderedMapStr>::GetGlobalPropagator();  // return noop-propagator

  const UnorderedMapStr carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},
      {"tracestate", "congo=t61rcWkgMzE"}};
  context::Context ctx1 = context::Context{
      "current-span",
      nostd::shared_ptr<trace::Span>(new trace::DefaultSpan(trace::SpanContext::GetInvalid()))};
  context::Context ctx2 = propagator->Extract(Getter, carrier, ctx1);
  UnorderedMapStr c2    = {};
  propagator->Inject(Setter, c2, ctx2);

  EXPECT_TRUE(c2.count("tracestate") == 0);
  EXPECT_TRUE(c2.count("traceparent") == 0);
}
