#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/composite_propagator.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using namespace opentelemetry;

template <typename T>
static std::string Hex(const T &id_item)
{
  char buf[T::kSize * 2];
  id_item.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}

static nostd::string_view Getter(const std::map<std::string, std::string> &carrier,
                                 nostd::string_view trace_type)
{
  auto it = carrier.find(std::string(trace_type));
  if (it != carrier.end())
  {
    return nostd::string_view(it->second);
  }
  return "";
}

static void Setter(std::map<std::string, std::string> &carrier,
                   nostd::string_view trace_type,
                   nostd::string_view trace_description = "")
{
  carrier[std::string(trace_type)] = std::string(trace_description);
}

using MapHttpTraceContext =
    trace::propagation::HttpTraceContext<std::map<std::string, std::string>>;
using MapB3Context = trace::propagation::B3Propagator<std::map<std::string, std::string>>;
using MapCompositePropagator =
    trace::propagation::CompositePropagator<std::map<std::string, std::string>>;

class CompositePropagatorTest : public ::testing::Test
{
  using MapHttpTraceContext =
      trace::propagation::HttpTraceContext<std::map<std::string, std::string>>;

  using MapB3Context = trace::propagation::B3Propagator<std::map<std::string, std::string>>;
  using MapCompositePropagator =
      trace::propagation::CompositePropagator<std::map<std::string, std::string>>;

public:
  CompositePropagatorTest()
  {
    std::vector<
        std::unique_ptr<trace::propagation::TextMapPropagator<std::map<std::string, std::string>>>>
        propogator_list = {};
    std::unique_ptr<MapHttpTraceContext> tc_propogator(new MapHttpTraceContext());
    std::unique_ptr<MapB3Context> b3_propogator(new MapB3Context());
    propogator_list.push_back(std::move(tc_propogator));
    propogator_list.push_back(std::move(b3_propogator));

    composite_propagator_ = new MapCompositePropagator(std::move(propogator_list));
  }

  ~CompositePropagatorTest() { delete composite_propagator_; }

protected:
  MapCompositePropagator *composite_propagator_;
};

TEST_F(CompositePropagatorTest, Extract)
{

  const std::map<std::string, std::string> carrier = {
      {"traceparent", "00-4bf92f3577b34da6a3ce929d0e0e4736-0102030405060708-01"},
      {"b3", "80f198ee56343ba864fe8b2a57d3eff7-e457b5a2e4d86bd1-1-05e3ac9a4f6e3b90"}};
  context::Context ctx1 = context::Context{};

  context::Context ctx2 = composite_propagator_->Extract(Getter, carrier, ctx1);

  auto ctx2_span = ctx2.GetValue(trace::kSpanKey);
  EXPECT_TRUE(nostd::holds_alternative<nostd::shared_ptr<trace::Span>>(ctx2_span));
  
  auto span = nostd::get<nostd::shared_ptr<trace::Span>>(ctx2_span);

  // confirm last propagator in composite propagator list (B3 here) wins for same key
  // ("active_span" here).
  EXPECT_EQ(Hex(span->GetContext().trace_id()), "80f198ee56343ba864fe8b2a57d3eff7");
  EXPECT_EQ(Hex(span->GetContext().span_id()), "e457b5a2e4d86bd1");
  EXPECT_EQ(span->GetContext().IsSampled(), true);
  EXPECT_EQ(span->GetContext().IsRemote(), true);
}

TEST_F(CompositePropagatorTest, Inject)
{

  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  trace::SpanContext span_context{trace::TraceId{buf_trace}, trace::SpanId{buf_span},
                                  trace::TraceFlags{true}, false};
  nostd::shared_ptr<trace::Span> sp{new trace::DefaultSpan{span_context}};

  // Set `sp` as the currently active span, which must be used by `Inject`.
  trace::Scope scoped_span{sp};

  std::map<std::string, std::string> headers = {};
  composite_propagator_->Inject(Setter, headers, context::RuntimeContext::GetCurrent());
  EXPECT_EQ(headers["traceparent"], "00-0102030405060708090a0b0c0d0e0f10-0102030405060708-01");
  EXPECT_EQ(headers["b3"], "0102030405060708090a0b0c0d0e0f10-0102030405060708-1");
}
