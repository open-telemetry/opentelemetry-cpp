#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/nostd/type_traits.h"
#include <opentelemetry/version.h>

#include <sstream>
#include <iostream>
#include <map>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
using SpanDataAttributeValue = nostd::variant<bool,
                                              int64_t,
                                              uint64_t,
                                              double,
                                              std::string,
                                              std::vector<bool>,
                                              std::vector<int64_t>,
                                              std::vector<uint64_t>,
                                              std::vector<double>,
                                              std::vector<std::string>>;

// AttributeType to help with printing attributes
enum AttributeType
{
  TYPE_BOOL,
  TYPE_INT64,
  TYPE_UINT64,
  TYPE_DOUBLE,
  TYPE_STRING,
  TYPE_VECTOR_BOOL,
  TYPE_VECTOR_INT64,
  TYPE_VECTOR_UINT64,
  TYPE_VECTOR_DOUBLE,
  TYPE_VECTOR_STRING
};

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

/**
 * The StreamSpanExporter exports span data through an ostream
*/
class StreamSpanExporter final : public sdktrace::SpanExporter
{
public:

/**
 * Create a StreamSpanExporter. This constructor takes in a reference to an ostream that the
 * export() function will send span data into.
 * The default ostream is set to stdout
 */
  explicit StreamSpanExporter(std::ostream &sout = std::cout) noexcept;

  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept override;

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  std::ostream &sout_;
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
  void print_array(SpanDataAttributeValue &value, bool jsonTypes = false)
  {
    sout_ << '[';
    // TODO: jsonTypes for bool?
    // TODO: do we need to escape string value for JSON?
    auto s    = nostd::get<std::vector<T>>(value);
    size_t i  = 1;
    size_t sz = s.size();
    for (auto v : s)
    {
      sout_ << v; // TODO: nostd::string_view type needs quotes!
      if (i != sz)
        sout_ << ',';
      i++;
    };
    sout_ << ']';
  }

  void print_value(SpanDataAttributeValue &value,
                          bool jsonTypes = false)
  {
    switch (value.index())
    {
      case AttributeType::TYPE_BOOL:
        if (jsonTypes)
        {
          sout_ << (nostd::get<bool>(value) ? "true" : "false");
        }
        else
        {
          sout_ << static_cast<unsigned>(nostd::get<bool>(value));
        }
        break;
      case AttributeType::TYPE_INT64:
        sout_ << nostd::get<int64_t>(value);
        break;
      case AttributeType::TYPE_UINT64:
        sout_ << nostd::get<uint64_t>(value);
        break;
      case AttributeType::TYPE_DOUBLE:
        sout_ << nostd::get<double>(value);
        break;
      case AttributeType::TYPE_STRING:
        if (jsonTypes)
          sout_ << '"';
        // TODO: do we need to escape string value for JSON?
        sout_ << nostd::get<std::string>(value);
        if (jsonTypes)
          sout_ << '"';
        break;
      case AttributeType::TYPE_VECTOR_BOOL:
        print_array<bool>(value, jsonTypes);
        break;
      case AttributeType::TYPE_VECTOR_INT64:
        print_array<int64_t>(value, jsonTypes);
        break;
      case AttributeType::TYPE_VECTOR_UINT64:
        print_array<uint64_t>(value, jsonTypes);
        break;
      case AttributeType::TYPE_VECTOR_DOUBLE:
        print_array<double>(value, jsonTypes);
        break;
      case AttributeType::TYPE_VECTOR_STRING:
        // TODO: print_array doesn't provide the proper quotes
        print_array<std::string>(value, jsonTypes);
        break;
    }
  }

  void printAttributes(std::unordered_map<std::string, SpanDataAttributeValue> map)
  {
    for(auto kv : map)
    {
      sout_ << kv.first << ": ";
      print_value(kv.second);
      sout_ << " ";
    }

    sout_ << "\n}\n";
  }
  
};
}
}
OPENTELEMETRY_END_NAMESPACE