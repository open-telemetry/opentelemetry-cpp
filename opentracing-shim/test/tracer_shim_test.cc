/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tracer_shim.h"
#include "span_shim.h"
#include "span_context_shim.h"
#include "shim_utils.h"
#include "shim_mocks.h"

#include <opentracing/noop.h>

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
  MockPropagator* text_map_format;
  MockPropagator* http_headers_format;

protected:
  virtual void SetUp()
  {
    using context::propagation::TextMapPropagator;

    text_map_format = new MockPropagator();
    http_headers_format = new MockPropagator();

    tracer_shim = shim::TracerShim::createTracerShim(trace_api::Provider::GetTracerProvider(), {
      .text_map = nostd::shared_ptr<TextMapPropagator>(text_map_format),
      .http_headers = nostd::shared_ptr<TextMapPropagator>(http_headers_format)
    });
  }

  virtual void TearDown()
  {
    tracer_shim.reset();
  }
};

TEST_F(TracerShimTest, SpanReferenceToCreatingTracer)
{
  auto span_shim = tracer_shim->StartSpan("a");
  ASSERT_NE(span_shim, nullptr);
  ASSERT_EQ(&span_shim->tracer(), tracer_shim.get());
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

TEST_F(TracerShimTest, InjectInvalidCarrier)
{
  auto span_shim = tracer_shim->StartSpan("a");
  auto result = tracer_shim->Inject(span_shim->context(), std::cout);
  ASSERT_TRUE(opentracing::are_errors_equal(result.error(), opentracing::invalid_carrier_error));
}

TEST_F(TracerShimTest, InjectNullContext)
{
  std::unordered_map<std::string, std::string> text_map;
  auto noop_tracer = opentracing::MakeNoopTracer();
  auto span = noop_tracer->StartSpan("a");
  auto result = tracer_shim->Inject(span->context(), TextMapCarrier{text_map});
  ASSERT_TRUE(result.has_value());
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

  auto span_context_shim = dynamic_cast<shim::SpanContextShim*>(span_context.value().get());
  ASSERT_TRUE(span_context_shim != nullptr);
  ASSERT_FALSE(span_context_shim->context().IsValid());
  ASSERT_FALSE(shim::utils::isBaggageEmpty(span_context_shim->baggage()));

  std::string value;
  ASSERT_TRUE(span_context_shim->BaggageItem("foo", value));
  ASSERT_EQ(value, "bar");
}
