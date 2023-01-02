// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "tracer_shim.h"

#include <opentracing/noop.h>

#include <gtest/gtest.h>

namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace context   = opentelemetry::context;
namespace shim      = opentelemetry::opentracingshim;

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

protected:
  virtual void SetUp()
  {
    tracer_shim = shim::TracerShim::createTracerShim();
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
