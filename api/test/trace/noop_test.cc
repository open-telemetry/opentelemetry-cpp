// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"

namespace trace_api = opentelemetry::trace;
namespace nonstd    = opentelemetry::nostd;
namespace common    = opentelemetry::common;

TEST(NoopTest, UseNoopTracers)
{
  std::shared_ptr<trace_api::Tracer> tracer{new trace_api::NoopTracer{}};
  auto s1 = tracer->StartSpan("abc");

  std::map<std::string, std::string> attributes1;
  s1->AddEvent("abc", attributes1);

  std::vector<std::pair<std::string, int>> attributes2;
  s1->AddEvent("abc", attributes2);

  s1->AddEvent("abc", {{"a", 1}, {"b", "2"}, {"c", 3.0}});

  std::vector<std::pair<std::string, std::vector<int>>> attributes3;
  s1->AddEvent("abc", attributes3);

  s1->SetAttribute("abc", 4);

  s1->AddEvent("abc");  // add Empty

  EXPECT_EQ(s1->IsRecording(), false);

  s1->SetStatus(trace_api::StatusCode::kUnset, "span unset");

  s1->UpdateName("test_name");

  common::SystemTimestamp t1;
  s1->AddEvent("test_time_stamp", t1);

  s1->GetContext();
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
TEST(NoopTest, UseNoopTracersAbiv2)
{
  std::shared_ptr<trace_api::Tracer> tracer{new trace_api::NoopTracer{}};
  auto s1 = tracer->StartSpan("abc");

  EXPECT_EQ(s1->IsRecording(), false);

  trace_api::SpanContext target(false, false);
  s1->AddLink(target, {{"noop1", 1}});

  s1->AddLinks({{trace_api::SpanContext(false, false), {{"noop2", 2}}}});

  EXPECT_FALSE(tracer->Enabled());
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO >= 2 */

TEST(NoopTest, StartSpan)
{
  std::shared_ptr<trace_api::Tracer> tracer{new trace_api::NoopTracer{}};

  std::map<std::string, std::string> attrs = {{"a", "3"}};
  std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>> links = {
      {trace_api::SpanContext(false, false), attrs}};
  auto s1 = tracer->StartSpan("abc", attrs, links);

  auto s2 =
      tracer->StartSpan("efg", {{"a", 3}}, {{trace_api::SpanContext(false, false), {{"b", 4}}}});
}

TEST(NoopTest, CreateSpanValidSpanContext)
{
  // Create valid spancontext for NoopSpan

  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  auto trace_id                 = trace_api::TraceId{buf_trace};
  auto span_id                  = trace_api::SpanId{buf_span};
  auto span_context             = nonstd::unique_ptr<trace_api::SpanContext>(
      new trace_api::SpanContext{trace_id, span_id, trace_api::TraceFlags{true}, false});
  auto s1 = nonstd::shared_ptr<trace_api::Span>(new trace_api::NoopSpan(std::move(span_context)));
  auto stored_span_context = s1->GetContext();
  EXPECT_EQ(stored_span_context.span_id(), span_id);
  EXPECT_EQ(stored_span_context.trace_id(), trace_id);

  s1->AddEvent("even1");  // noop
  s1->End();              // noop
}

TEST(NoopTest, StartSpanPreservesParentSpanContext)
{
  constexpr uint8_t buf_span[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t buf_trace[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  const trace_api::SpanContext parent{trace_api::TraceId{buf_trace}, trace_api::SpanId{buf_span},
                                      trace_api::TraceFlags{true}, false};
  std::shared_ptr<trace_api::Tracer> tracer{new trace_api::NoopTracer()};

  trace_api::StartSpanOptions parent_options;
  parent_options.parent = parent;
  auto child            = tracer->StartSpan("child", parent_options);
  EXPECT_FALSE(child->IsRecording());
  EXPECT_EQ(child->GetContext(), parent);

  auto parent_span = nonstd::shared_ptr<trace_api::Span>(new trace_api::NoopSpan(
      nonstd::unique_ptr<trace_api::SpanContext>(new trace_api::SpanContext(parent))));
  opentelemetry::context::Context context;
  context = trace_api::SetSpan(context, parent_span);

  trace_api::StartSpanOptions context_options;
  context_options.parent  = context;
  auto child_from_context = tracer->StartSpan("child", context_options);
  EXPECT_FALSE(child_from_context->IsRecording());
  EXPECT_EQ(child_from_context->GetContext(), parent);

  auto context_with_span_context = opentelemetry::context::Context{
      trace_api::kSpanKey,
      nonstd::shared_ptr<trace_api::SpanContext>(new trace_api::SpanContext(parent))};
  context_options.parent              = context_with_span_context;
  auto child_from_stored_span_context = tracer->StartSpan("child", context_options);
  EXPECT_FALSE(child_from_stored_span_context->IsRecording());
  EXPECT_EQ(child_from_stored_span_context->GetContext(), parent);

  {
    trace_api::Scope scope(parent_span);
    auto child_from_current_context = tracer->StartSpan("child");
    EXPECT_FALSE(child_from_current_context->IsRecording());
    EXPECT_EQ(child_from_current_context->GetContext(), parent);

    trace_api::StartSpanOptions invalid_span_context_options;
    invalid_span_context_options.parent  = trace_api::SpanContext::GetInvalid();
    auto child_from_invalid_span_context = tracer->StartSpan("child", invalid_span_context_options);
    EXPECT_FALSE(child_from_invalid_span_context->IsRecording());
    EXPECT_EQ(child_from_invalid_span_context->GetContext(), parent);

    trace_api::StartSpanOptions empty_context_options;
    empty_context_options.parent  = opentelemetry::context::Context{};
    auto child_from_empty_context = tracer->StartSpan("child", empty_context_options);
    EXPECT_FALSE(child_from_empty_context->IsRecording());
    EXPECT_EQ(child_from_empty_context->GetContext(), parent);

    trace_api::StartSpanOptions invalid_span_options;
    invalid_span_options.parent = opentelemetry::context::Context{
        trace_api::kSpanKey, nonstd::shared_ptr<trace_api::Span>(new trace_api::NoopSpan())};
    auto child_from_invalid_span = tracer->StartSpan("child", invalid_span_options);
    EXPECT_FALSE(child_from_invalid_span->IsRecording());
    EXPECT_EQ(child_from_invalid_span->GetContext(), parent);

    trace_api::StartSpanOptions root_context_options;
    root_context_options.parent = opentelemetry::context::Context{trace_api::kIsRootSpanKey, true};
    auto root_child             = tracer->StartSpan("root", root_context_options);
    EXPECT_FALSE(root_child->IsRecording());
    EXPECT_FALSE(root_child->GetContext().IsValid());
  }
}
