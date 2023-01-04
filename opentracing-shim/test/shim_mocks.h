/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <opentelemetry/baggage/baggage_context.h>
#include <opentracing/propagation.h>

#include <unordered_map>

namespace baggage = opentelemetry::baggage;
namespace context = opentelemetry::context;
namespace nostd   = opentelemetry::nostd;

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
