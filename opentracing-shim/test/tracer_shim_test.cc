// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "tracer_shim.h"
#include "span_context_shim.h"

#include <opentelemetry/baggage/baggage_context.h>
#include <opentracing/noop.h>

#include <gtest/gtest.h>

namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace context   = opentelemetry::context;
namespace baggage   = opentelemetry::baggage;
namespace shim      = opentelemetry::opentracingshim;

struct MockPropagator : public context::propagation::TextMapPropagator
{
  // Returns the context that is stored in the carrier with the TextMapCarrier as extractor.
  context::Context Extract(const context::propagation::TextMapCarrier &carrier,
                           context::Context &context) noexcept override
  {
    std::vector<std::pair<std::string, std::string>> kvs; 
    carrier.Keys([&carrier,&kvs](nostd::string_view k){ 
      kvs.emplace_back(k, carrier.Get(k));
      return true;
    });
    is_extracted = true;
    return baggage::SetBaggage(context, nostd::shared_ptr<baggage::Baggage>(new baggage::Baggage(kvs)));
  }

  // Sets the context for carrier with self defined rules.
  void Inject(context::propagation::TextMapCarrier &carrier,
              const context::Context &context) noexcept override
  {
    auto baggage = baggage::GetBaggage(context);
    baggage->GetAllEntries([&carrier](nostd::string_view k, nostd::string_view v){
      carrier.Set(k, v);
      return true;
    });
    is_injected = true;
  }

  // Gets the fields set in the carrier by the `inject` method
  bool Fields(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    return true;
  }

  bool is_extracted = false;
  bool is_injected = false;
};

struct TextMapCarrier : opentracing::TextMapReader, opentracing::TextMapWriter {
  TextMapCarrier(std::unordered_map<std::string, std::string>& text_map_)
      : text_map(text_map_) {}

  opentracing::expected<void> Set(opentracing::string_view key, opentracing::string_view value) const override {
    text_map[key] = value;
    return {};
  }

  opentracing::expected<opentracing::string_view> LookupKey(opentracing::string_view key) const override {
    if (!supports_lookup) {
      return opentracing::make_unexpected(opentracing::lookup_key_not_supported_error);
    }
    auto iter = text_map.find(key);
    if (iter != text_map.end()) {
      return opentracing::string_view{iter->second};
    } else {
      return opentracing::make_unexpected(opentracing::key_not_found_error);
    }
  }

  opentracing::expected<void> ForeachKey(
      std::function<opentracing::expected<void>(opentracing::string_view key, opentracing::string_view value)> f)
      const override {
    ++foreach_key_call_count;
    for (const auto& key_value : text_map) {
      auto result = f(key_value.first, key_value.second);
      if (!result) return result;
    }
    return {};
  }

  bool supports_lookup = false;
  mutable int foreach_key_call_count = 0;
  std::unordered_map<std::string, std::string>& text_map;
};

struct HTTPHeadersCarrier : opentracing::HTTPHeadersReader, opentracing::HTTPHeadersWriter {
  HTTPHeadersCarrier(std::unordered_map<std::string, std::string>& text_map_)
      : text_map(text_map_) {}

  opentracing::expected<void> Set(opentracing::string_view key, opentracing::string_view value) const override {
    text_map[key] = value;
    return {};
  }

  opentracing::expected<void> ForeachKey(
      std::function<opentracing::expected<void>(opentracing::string_view key, opentracing::string_view value)> f)
      const override {
    for (const auto& key_value : text_map) {
      auto result = f(key_value.first, key_value.second);
      if (!result) return result;
    }
    return {};
  }

  std::unordered_map<std::string, std::string>& text_map;
};

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

    tracer_shim = shim::TracerShim::createTracerShim(trace_api::Provider::GetTracerProvider(),
      { .text_map = nostd::shared_ptr<TextMapPropagator>(text_map_format),
        .http_headers = nostd::shared_ptr<TextMapPropagator>(http_headers_format) });
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

  TextMapCarrier carrier{text_map};
  tracer_shim->Inject(span_shim->context(), carrier);

  auto span_context = tracer_shim->Extract(carrier);
  ASSERT_TRUE(span_context.value() != nullptr);

  // auto span_context_shim = dynamic_cast<shim::SpanContextShim*>(span_context.value().get());
  // ASSERT_TRUE(span_context_shim != nullptr);
  // ASSERT_TRUE(span_context_shim->context().IsValid());

  // std::string value;
  // ASSERT_TRUE(span_context_shim->BaggageItem("foo", value));
  // ASSERT_EQ(value, "bar");
}
