/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <opentelemetry/baggage/baggage.h>
#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/nostd/type_traits.h>
#include <opentracing/propagation.h>
#include <opentracing/value.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim::utils
{

static inline opentelemetry::common::AttributeValue attributeFromValue(const opentracing::Value& value)
{
  using opentelemetry::common::AttributeValue;

  static struct
  {
    AttributeValue operator()(bool v) { return v; }
    AttributeValue operator()(double v) { return v; }
    AttributeValue operator()(int64_t v) { return v; }
    AttributeValue operator()(uint64_t v) { return v; }
    AttributeValue operator()(std::string v) { return v.c_str(); }
    AttributeValue operator()(opentracing::string_view v) { return nostd::string_view{v.data()}; }
    AttributeValue operator()(std::nullptr_t) { return nostd::string_view{}; }
    AttributeValue operator()(const char* v) { return v; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Values>) { return nostd::string_view{}; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>) { return nostd::string_view{}; }
  } AttributeMapper;

  return opentracing::Value::visit(value, AttributeMapper);
}

static inline std::string stringFromValue(const opentracing::Value& value)
{
  static struct
  {
    std::string operator()(bool v) { return v ? "true" : "false"; }
    std::string operator()(double v) { return std::to_string(v); }
    std::string operator()(int64_t v) { return std::to_string(v); }
    std::string operator()(uint64_t v) { return std::to_string(v); }
    std::string operator()(std::string v) { return v; }
    std::string operator()(opentracing::string_view v) { return std::string{v.data()}; }
    std::string operator()(std::nullptr_t) { return std::string{}; }
    std::string operator()(const char* v) { return std::string{v}; }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Values>) { return std::string{}; }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>) { return std::string{}; }
  } StringMapper;

  return opentracing::Value::visit(value, StringMapper);
}

template<typename T, nostd::enable_if_t<std::is_base_of<opentracing::TextMapWriter, T>::value, bool> = true>
class CarrierWriterShim : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  CarrierWriterShim(const T& writer) : writer_(writer) {}

  // returns the value associated with the passed key.
  virtual nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    return "";
  }

  // stores the key-value pair.
  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    writer_.Set(key.data(), value.data());
  }

private:
  const T& writer_;
};

template<typename T, nostd::enable_if_t<std::is_base_of<opentracing::TextMapReader, T>::value, bool> = true>
class CarrierReaderShim : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  CarrierReaderShim(const T& reader) : reader_(reader) {}

  // returns the value associated with the passed key.
  virtual nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    nostd::string_view value;

    // First try carrier.LookupKey since that can potentially be the fastest approach.
    if (auto result = reader_.LookupKey(key.data()))
    {
      value = result.value().data();
    }
    else // Fall back to iterating through all of the keys.
    {
      reader_.ForeachKey([key, &value]
        (opentracing::string_view k, opentracing::string_view v) -> opentracing::expected<void> {
          if (k == key.data())
          {
            value = v.data();
            // Found key, so bail out of the loop with a success error code.
            return opentracing::make_unexpected(std::error_code{});
          }
          return opentracing::make_expected();
        });
    }

    return value;
  }

  // stores the key-value pair.
  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    // Not required for Opentracing reader
  }

  // list of all the keys in the carrier.
  virtual bool Keys(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    return reader_.ForeachKey([&callback]
      (opentracing::string_view key, opentracing::string_view) -> opentracing::expected<void> {
        return callback(key.data())
          ? opentracing::make_expected()
          : opentracing::make_unexpected(std::error_code{});
      }).has_value();
  }

private:
  const T& reader_;
};

static inline bool isBaggageEmpty(const nostd::shared_ptr<opentelemetry::baggage::Baggage>& baggage)
{
  if (baggage)
  {
    return baggage->GetAllEntries([](nostd::string_view, nostd::string_view){
      return false;
    });
  }

  return true;
}

} // namespace opentracingshim::utils
OPENTELEMETRY_END_NAMESPACE
