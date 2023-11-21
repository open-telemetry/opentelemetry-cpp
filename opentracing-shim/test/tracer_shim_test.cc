/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "shim_mocks.h"

#include "opentelemetry/opentracingshim/shim_utils.h"
#include "opentelemetry/opentracingshim/span_context_shim.h"
#include "opentelemetry/opentracingshim/span_shim.h"
#include "opentelemetry/opentracingshim/tracer_shim.h"

#include "opentracing/noop.h"

#include <gtest/gtest.h>

namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace context   = opentelemetry::context;
namespace baggage   = opentelemetry::baggage;
namespace shim      = opentelemetry::opentracingshim;

class TracerShimTest : public testing::Test
{
public:
  std::shared_ptr<opentracing::Tracer> tracer_shim;
  MockPropagator *text_map_format;
  MockPropagator *http_headers_format;

protected:
  virtual void SetUp() override
  {
    using context::propagation::TextMapPropagator;

    text_map_format     = new MockPropagator();
    http_headers_format = new MockPropagator();

    tracer_shim = shim::TracerShim::createTracerShim(
        trace_api::Provider::GetTracerProvider(),
        {.text_map     = nostd::shared_ptr<TextMapPropagator>(text_map_format),
         .http_headers = nostd::shared_ptr<TextMapPropagator>(http_headers_format)});
  }

  virtual void TearDown() override { tracer_shim.reset(); }
};

TEST_F(TracerShimTest, TracerName)
{
  auto mock_provider_ptr = new MockTracerProvider();
  nostd::shared_ptr<trace_api::TracerProvider> provider(mock_provider_ptr);
  ASSERT_NE(shim::TracerShim::createTracerShim(provider), nullptr);
  ASSERT_EQ(mock_provider_ptr->library_name_, "opentracing-shim");
}

TEST_F(TracerShimTest, SpanReferenceToCreatingTracer)
{
  auto span_shim = tracer_shim->StartSpan("a");
  ASSERT_NE(span_shim, nullptr);
  ASSERT_EQ(&span_shim->tracer(), tracer_shim.get());
}

TEST_F(TracerShimTest, SpanParentChildRelationship)
{
  auto span_shim1 = tracer_shim->StartSpan("a");
  auto span_shim2 = tracer_shim->StartSpan("b", {opentracing::ChildOf(&span_shim1->context())});
  ASSERT_NE(span_shim1, nullptr);
  ASSERT_NE(span_shim2, nullptr);
  ASSERT_NE(span_shim1, span_shim2);
  ASSERT_EQ(span_shim1->context().ToSpanID(), span_shim2->context().ToSpanID());
  ASSERT_EQ(span_shim1->context().ToTraceID(), span_shim2->context().ToTraceID());

  auto span_context_shim1 = static_cast<const shim::SpanContextShim *>(&span_shim1->context());
  auto span_context_shim2 = static_cast<const shim::SpanContextShim *>(&span_shim2->context());
  ASSERT_TRUE(span_context_shim1 != nullptr);
  ASSERT_TRUE(span_context_shim2 != nullptr);
  ASSERT_EQ(span_context_shim1->context(), span_context_shim2->context());
}

TEST_F(TracerShimTest, TracerGloballyRegistered)
{
  ASSERT_FALSE(opentracing::Tracer::IsGlobalTracerRegistered());
  ASSERT_NE(opentracing::Tracer::InitGlobal(tracer_shim), nullptr);
  ASSERT_TRUE(opentracing::Tracer::IsGlobalTracerRegistered());
}

TEST_F(TracerShimTest, Close)
{
  tracer_shim->Close();
  auto span_shim = tracer_shim->StartSpan("a");
  ASSERT_TRUE(span_shim == nullptr);
}

TEST_F(TracerShimTest, SpanHandleErrorTagAtCreation)
{
  auto mock_provider_ptr = new MockTracerProvider();
  nostd::shared_ptr<trace_api::TracerProvider> provider(mock_provider_ptr);
  auto tracer_shim = shim::TracerShim::createTracerShim(provider);
  auto span_shim   = tracer_shim->StartSpanWithOptions("test", {});
  ASSERT_TRUE(span_shim != nullptr);
  ASSERT_TRUE(mock_provider_ptr->tracer_ != nullptr);
  ASSERT_TRUE(mock_provider_ptr->tracer_->span_ != nullptr);
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->name_, "test");
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->status_.first, trace_api::StatusCode::kUnset);

  auto span_shim1 = tracer_shim->StartSpanWithOptions("test1", {.tags = {{"event", "normal"}}});
  ASSERT_TRUE(span_shim1 != nullptr);
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->name_, "test1");
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->status_.first, trace_api::StatusCode::kUnset);

  auto span_shim2 = tracer_shim->StartSpanWithOptions("test2", {.tags = {{"error", true}}});
  ASSERT_TRUE(span_shim2 != nullptr);
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->name_, "test2");
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->status_.first, trace_api::StatusCode::kError);

  auto span_shim3 = tracer_shim->StartSpanWithOptions("test3", {.tags = {{"error", "false"}}});
  ASSERT_TRUE(span_shim3 != nullptr);
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->name_, "test3");
  ASSERT_EQ(mock_provider_ptr->tracer_->span_->status_.first, trace_api::StatusCode::kOk);
}

TEST_F(TracerShimTest, InjectInvalidCarrier)
{
  auto span_shim = tracer_shim->StartSpan("a");
  auto result    = tracer_shim->Inject(span_shim->context(), std::cout);
  ASSERT_TRUE(opentracing::are_errors_equal(result.error(), opentracing::invalid_carrier_error));
}

TEST_F(TracerShimTest, InjectNullContext)
{
  std::unordered_map<std::string, std::string> text_map;
  auto noop_tracer = opentracing::MakeNoopTracer();
  auto span        = noop_tracer->StartSpan("a");
  auto result      = tracer_shim->Inject(span->context(), TextMapCarrier{text_map});
  ASSERT_TRUE(
      opentracing::are_errors_equal(result.error(), opentracing::invalid_span_context_error));
  ASSERT_TRUE(text_map.empty());
}

TEST_F(TracerShimTest, InjectTextMap)
{
  ASSERT_FALSE(text_map_format->is_injected);
  ASSERT_FALSE(http_headers_format->is_injected);

  std::unordered_map<std::string, std::string> text_map;
  auto span_shim = tracer_shim->StartSpan("a");
  tracer_shim->Inject(span_shim->context(), TextMapCarrier{text_map});
  ASSERT_TRUE(text_map_format->is_injected);
  ASSERT_FALSE(http_headers_format->is_injected);
}

TEST_F(TracerShimTest, InjectHttpsHeaders)
{
  ASSERT_FALSE(text_map_format->is_injected);
  ASSERT_FALSE(http_headers_format->is_injected);

  std::unordered_map<std::string, std::string> text_map;
  auto span_shim = tracer_shim->StartSpan("a");
  tracer_shim->Inject(span_shim->context(), HTTPHeadersCarrier{text_map});
  ASSERT_FALSE(text_map_format->is_injected);
  ASSERT_TRUE(http_headers_format->is_injected);
}

TEST_F(TracerShimTest, ExtractInvalidCarrier)
{
  auto result = tracer_shim->Extract(std::cin);
  ASSERT_TRUE(opentracing::are_errors_equal(result.error(), opentracing::invalid_carrier_error));
}

TEST_F(TracerShimTest, ExtractNullContext)
{
  std::unordered_map<std::string, std::string> text_map;
  auto result = tracer_shim->Extract(TextMapCarrier{text_map});
  ASSERT_EQ(result.value(), nullptr);
}

TEST_F(TracerShimTest, ExtractTextMap)
{
  ASSERT_FALSE(text_map_format->is_extracted);
  ASSERT_FALSE(http_headers_format->is_extracted);

  std::unordered_map<std::string, std::string> text_map;
  auto result = tracer_shim->Extract(TextMapCarrier{text_map});
  ASSERT_EQ(result.value(), nullptr);
  ASSERT_TRUE(text_map_format->is_extracted);
  ASSERT_FALSE(http_headers_format->is_extracted);
}

TEST_F(TracerShimTest, ExtractHttpsHeaders)
{
  ASSERT_FALSE(text_map_format->is_extracted);
  ASSERT_FALSE(http_headers_format->is_extracted);

  std::unordered_map<std::string, std::string> text_map;
  auto result = tracer_shim->Extract(HTTPHeadersCarrier{text_map});
  ASSERT_EQ(result.value(), nullptr);
  ASSERT_FALSE(text_map_format->is_extracted);
  ASSERT_TRUE(http_headers_format->is_extracted);
}

TEST_F(TracerShimTest, ExtractOnlyBaggage)
{
  std::unordered_map<std::string, std::string> text_map;
  auto span_shim = tracer_shim->StartSpan("a");
  span_shim->SetBaggageItem("foo", "bar");
  ASSERT_EQ(span_shim->BaggageItem("foo"), "bar");

  tracer_shim->Inject(span_shim->context(), TextMapCarrier{text_map});
  auto span_context = tracer_shim->Extract(TextMapCarrier{text_map});
  ASSERT_TRUE(span_context.value() != nullptr);

  auto span_context_shim = static_cast<shim::SpanContextShim *>(span_context.value().get());
  ASSERT_TRUE(span_context_shim != nullptr);
  ASSERT_FALSE(span_context_shim->context().IsValid());
  ASSERT_FALSE(span_context_shim->context().IsSampled());
  ASSERT_FALSE(shim::utils::isBaggageEmpty(span_context_shim->baggage()));

  std::string value;
  ASSERT_TRUE(span_context_shim->BaggageItem("foo", value));
  ASSERT_EQ(value, "bar");
}
