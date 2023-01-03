/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim::shimutils
{
    
using opentelemetry::common::AttributeValue;

static inline AttributeValue attributeFromValue(const opentracing::Value& value)
{
  static struct
  {
    AttributeValue operator()(bool v) { return v; }
    AttributeValue operator()(double v) { return v; }
    AttributeValue operator()(int64_t v) { return v; }
    AttributeValue operator()(uint64_t v) { return v; }
    AttributeValue operator()(std::string v) { return v.c_str(); }
    AttributeValue operator()(opentracing::string_view v) { return nostd::string_view{v.data()}; }
    AttributeValue operator()(std::nullptr_t) { return std::string{}; }
    AttributeValue operator()(const char* v) { return v; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Values>) { return std::string{}; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>) { return std::string{}; }
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
    // First try carrier.LookupKey since that can potentially be the fastest approach.
    auto result = reader_.LookupKey(key.data());

    if (!result.has_value() || 
        opentracing::are_errors_equal(result.error(), opentracing::lookup_key_not_supported_error)) 
    {
      // Fall back to iterating through all of the keys.
      reader_.ForeachKey([key, &result]
        (opentracing::string_view k, opentracing::string_view v) -> opentracing::expected<void> {
          if (key == k) 
          {
            result = opentracing::make_expected(v);
            // Found key, so bail out of the loop with a success error code.
            return opentracing::make_unexpected(std::error_code{});
          }
          return opentracing::make_expected();
        });
    }

    return nostd::string_view{ result.has_value() ? result.value().data() : "" };
  }

  // stores the key-value pair.
  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    // Not required for Opentracing reader
  }

  // list of all the keys in the carrier.
  virtual bool Keys(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    reader_.ForeachKey([&callback]
        (opentracing::string_view k, opentracing::string_view v) -> opentracing::expected<void> {
          callback(k.data());
          return opentracing::make_expected();
        });
    return true;
  }

private:
  const T& reader_;
};

static inline bool isBaggageEmpty(const BaggagePtr& baggage)
{
  if (baggage)
  {
    return baggage->GetAllEntries([](nostd::string_view, nostd::string_view){
      return false;
    });    
  }

  return true;
}

} // namespace opentracingshim::shimutils
OPENTELEMETRY_END_NAMESPACE
