#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <mutex>

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <opentelemetry/nostd/variant.h>
#include <opentelemetry/nostd/string_view.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <tuple>

#include "utils.hpp"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace protocol
{
/// <summary>
/// EventData type allows to aggregate Span name, Timestamp and Key-Value Attributes
/// </summary>
using EventData =
    std::tuple<nostd::string_view &, core::SystemTimestamp, const trace::KeyValueIterable &>;

/// <summary>
/// Utility function to append AttributeValue string representation to stream
/// </summary>
/// <param name="ss">Output stringstream</param>
/// <param name="value">Value to append</param>
/// <param name="jsonTypes">Whether to apply JSON-style quotes</param>
static void print_value(std::stringstream &ss,
                        common::AttributeValue &value,
                        bool jsonTypes = false)
{
  switch (value.index())
  {
    case common::AttributeType::TYPE_BOOL:
      if (jsonTypes)
      {
        ss << (nostd::get<bool>(value) ? "true" : "false");
      }
      else
      {
        ss << static_cast<unsigned>(nostd::get<bool>(value));
      }
      break;
    case common::AttributeType::TYPE_INT:
      ss << nostd::get<int32_t>(value);
      break;
    case common::AttributeType::TYPE_INT64:
      ss << nostd::get<int64_t>(value);
      break;
    case common::AttributeType::TYPE_UINT:
      ss << nostd::get<uint32_t>(value);
      break;
    case common::AttributeType::TYPE_UINT64:
      ss << nostd::get<uint64_t>(value);
      break;
    case common::AttributeType::TYPE_DOUBLE:
      ss << nostd::get<double>(value);
      break;
    case common::AttributeType::TYPE_STRING:
      if (jsonTypes)
        ss << '"';
      // TODO: do we need to escape string value for JSON?
      ss << nostd::get<nostd::string_view>(value);
      if (jsonTypes)
        ss << '"';
      break;
    case common::AttributeType::TYPE_CSTRING:
      if (jsonTypes)
        ss << '"';
      // TODO: do we need to escape string value for JSON?
      ss << nostd::get<const char *>(value);
      if (jsonTypes)
        ss << '"';
      break;
#ifdef HAVE_SPAN_BYTE
    case common::AttributeType::TYPE_SPAN_BYTE: {
      ss << '[';
      // TODO: do we need to escape string value for JSON?
      auto s    = nostd::get<nostd::span<const uint8_t>>(value);
      size_t i  = 1;
      size_t sz = s.size();
      for (auto v : s)
      {
        ss << (unsigned)v;
        if (i != sz)
          ss << ',';
        i++;
      };
      ss << ']';
      break;
    };
#endif
    default:
      /* TODO: unsupported type - add all other types here */
      break;
  }
};

///
/// Generic event data converter base class
///
struct EventConverter
{
  virtual std::string convert(EventData data) = 0;
};

///
/// NULL converter
///
struct NullConverter : EventConverter
{
  virtual std::string convert(EventData data) override { return ""; };
};

///
/// Converts event data to JSON
///
struct JsonConverter : EventConverter
{
  virtual std::string convert(EventData data) override
  {
    nostd::string_view &name                 = std::get<0>(data);
    std::chrono::system_clock::time_point tp = std::get<1>(data);
    auto &attributes                         = std::get<2>(data);

    std::stringstream ss;

    ss << "{"
       // TODO:
       // - decide on the best format for timestamp
       // - skip timestamp in low-latency agent-based env with sync events
       << "\"time\":\"" << OPENTELEMETRY_NAMESPACE::utils::to_string(tp)
       << "\","
       // TODO:
       // - different schemas inside JSON may name event name field differently
       << "\"name\":"
       << "\"" << name << "\"";

    size_t size = attributes.size();
    if (size)
    {
      ss << ',';
      size_t i = 1;
      // TODO: we need to do something with this iterator. It is not convenient.
      attributes.ForEachKeyValue(
          [&](nostd::string_view key, common::AttributeValue value) noexcept {
            ss << "\"" << key << "\":";
            print_value(ss, value, true);
            if (size != i)
            {
              ss << ",";
            }
            i++;
            return true;
          });
    };
    ss << "}";
    ss << std::endl;
    return ss.str();
  }
};

///
/// Converts event data to Comma Separated Values
///
struct PlainKVConverter : EventConverter
{

  virtual std::string convert(EventData data) override
  {
    nostd::string_view &name                 = std::get<0>(data);
    std::chrono::system_clock::time_point tp = std::get<1>(data);
    auto &attributes                         = std::get<2>(data);
    std::stringstream ss;

    ss << "time=\"" << OPENTELEMETRY_NAMESPACE::utils::to_string(tp) << "\", ";
    ss << "name="
       << "\"" << name << "\"";
    attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
      ss << ", ";
      ss << key << '=';
      print_value(ss, value, true);
      return true;
    });
    ss << std::endl;
    return ss.str();
  }
};

///
/// Converts event data to ETW event
///
struct ETWEventConverter : EventConverter
{

  // Generic unmanifested XML event formatter. This is temporary implementation
  // that allows to send 'structured' events using EventWriteString in-lieu of
  // Trace Logging Dynamic functionality for C++ code. Going forward this code
  // has to be replaced by cusstom operator << EventData on stream provider
  // that will use TraceLoggingDynamic.h to populate dynamic-manifested ETW.
  virtual std::string convert(EventData data) override
  {
    nostd::string_view &name                 = std::get<0>(data);
    std::chrono::system_clock::time_point tp = std::get<1>(data);
    auto &attributes                         = std::get<2>(data);

    std::stringstream ss;

    // Listener must implement the XML payload parser.
    // Reference example:
    // https://github.com/maxgolov/SilkETW/commit/6d036b5d5023cbbbdd6bfcbe02b411b42be6d6d5
    ss << "<Event name=\"" << name << "\" ";
    size_t size = attributes.size();
    if (size)
    {
      // TODO: we need to do something with this iterator. It is not convenient.
      attributes.ForEachKeyValue(
          [&](nostd::string_view key, common::AttributeValue value) noexcept {
            ss << ' ';
            // TODO:
            // - sanitize key to remove dots
            // - decide how to express strong types
            ss << key << "=\"";
            print_value(ss, value, false);
            ss << '"';
            return true;
          });
    }
    ss << "/>";
    auto s = ss.str();
    // Make sure that the buffer ends with wchar_t NUL = { 0x0000 }
    if (s.length() % 2)
    {
      // padding
      s += '\0';
    }
    else
    {
      // NUL
      s += '\0';
      s += '\0';
    }

    return s;
  }
};

};  // namespace protocol
OPENTELEMETRY_END_NAMESPACE
