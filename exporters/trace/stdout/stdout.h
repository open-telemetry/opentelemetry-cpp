#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/common/attribute_value.h"
#include <opentelemetry/version.h>

#include <sstream>
#include <iostream>
#include <map>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

// AttributeType to help with printing attributes
enum AttributeType
{
  TYPE_BOOL,
  TYPE_INT,
  TYPE_INT64,
  TYPE_UINT,
  TYPE_UINT64,
  TYPE_DOUBLE,
  TYPE_STRING,
  TYPE_SPAN_BOOL,
  TYPE_SPAN_INT,
  TYPE_SPAN_INT64,
  TYPE_SPAN_UINT,
  TYPE_SPAN_UINT64,
  TYPE_SPAN_DOUBLE,
  TYPE_SPAN_STRING
};

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{
class StdoutSpanExporter final : public sdktrace::SpanExporter
{
public:
  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept override;

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  bool isShutdown_ = false;

  // Mapping status number to the string from api/include/opentelemetry/trace/canonical_code.h
  std::map<int, std::string> statusMap {
    {0, "OK"},
    {1, "CANCELLED"},
    {2, "UNKNOWN"},
    {3, "INVALID_ARGUMENT"},
    {4, "DEADLINE_EXCEEDED"},
    {5, "NOT_FOUND"},
    {6, "ALREADY_EXISTS"},
    {7, "PERMISSION_DENIED"},
    {8, "RESOURCE_EXHAUSTED"},
    {9, "FAILED_PRECONDITION"},
    {10,"ABORTED"},
    {11,"OUT_OF_RANGE"},
    {12,"UNIMPLEMENTED"},
    {13,"INTERNAL"},
    {14,"UNAVAILABLE"},
    {15,"DATA_LOSS"},
    {16,"UNAUTHENTICATED"}
  };

  /*
    print_array and print_value are used to print out the value of an attribute within a span. These values
    are held in a variant which makes the process of printing them much more complicated.
  */

  template <typename T>
  static void print_array(std::stringstream &ss, common::AttributeValue &value, bool jsonTypes = false)
  {
    ss << '[';
    // TODO: jsonTypes for bool?
    // TODO: do we need to escape string value for JSON?
    auto s    = nostd::get<nostd::span<const T>>(value);
    size_t i  = 1;
    size_t sz = s.size();
    for (auto v : s)
    {
      ss << v; // TODO: nostd::string_view type needs quotes!
      if (i != sz)
        ss << ',';
      i++;
    };
    ss << ']';
  };

  static void print_value(std::stringstream &ss,
                          common::AttributeValue &value,
                          bool jsonTypes = false)
  {
    switch (value.index())
    {
      case AttributeType::TYPE_BOOL:
        if (jsonTypes)
        {
          ss << (nostd::get<bool>(value) ? "true" : "false");
        }
        else
        {
          ss << static_cast<unsigned>(nostd::get<bool>(value));
        }
        break;
      case AttributeType::TYPE_INT:
        ss << nostd::get<int>(value);
        break;
      case AttributeType::TYPE_INT64:
        ss << nostd::get<int64_t>(value);
        break;
      case AttributeType::TYPE_UINT:
        ss << nostd::get<unsigned int>(value);
        break;
      case AttributeType::TYPE_UINT64:
        ss << nostd::get<uint64_t>(value);
        break;
      case AttributeType::TYPE_DOUBLE:
        ss << nostd::get<double>(value);
        break;
      case AttributeType::TYPE_STRING:
        if (jsonTypes)
          ss << '"';
        // TODO: do we need to escape string value for JSON?
        ss << nostd::get<nostd::string_view>(value);
        if (jsonTypes)
          ss << '"';
        break;
      case AttributeType::TYPE_SPAN_BOOL:
        print_array<bool>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_INT:
        print_array<int>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_INT64:
        print_array<int64_t>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_UINT:
        print_array<unsigned int>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_UINT64:
        print_array<uint64_t>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_DOUBLE:
        print_array<double>(ss, value, jsonTypes);
        break;
      case AttributeType::TYPE_SPAN_STRING:
        // TODO: print_array doesn't provide the proper quotes
        print_array<nostd::string_view>(ss, value, jsonTypes);
        break;
    }
  }

  void printAttributes(std::unordered_map<std::string, common::AttributeValue> map)
  {
    for(auto kv : map)
    {
      std::stringstream ss;
      std::cout << kv.first << ": ";
      print_value(ss, kv.second, true);

      std::cout << ss.str() << " ";
    }

    std::cout << "\n}"
              << "\n";
  }
  
};
}
}
OPENTELEMETRY_END_NAMESPACE