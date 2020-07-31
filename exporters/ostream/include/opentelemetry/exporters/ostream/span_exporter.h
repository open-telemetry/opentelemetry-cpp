#pragma once

#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/version.h"

#include <iostream>
#include <map>
#include <sstream>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

/**
 * The OStreamSpanExporter exports span data through an ostream
 */
class OStreamSpanExporter final : public sdktrace::SpanExporter
{
public:
  /**
   * Create an OStreamSpanExporter. This constructor takes in a reference to an ostream that the
   * export() function will send span data into.
   * The default ostream is set to stdout
   */
  explicit OStreamSpanExporter(std::ostream &sout = std::cout) noexcept;

  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept override;

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  std::ostream &sout_;
  bool isShutdown_ = false;

  // Mapping status number to the string from api/include/opentelemetry/trace/canonical_code.h
  std::map<int, std::string> statusMap{{0, "OK"},
                                       {1, "CANCELLED"},
                                       {2, "UNKNOWN"},
                                       {3, "INVALID_ARGUMENT"},
                                       {4, "DEADLINE_EXCEEDED"},
                                       {5, "NOT_FOUND"},
                                       {6, "ALREADY_EXISTS"},
                                       {7, "PERMISSION_DENIED"},
                                       {8, "RESOURCE_EXHAUSTED"},
                                       {9, "FAILED_PRECONDITION"},
                                       {10, "ABORTED"},
                                       {11, "OUT_OF_RANGE"},
                                       {12, "UNIMPLEMENTED"},
                                       {13, "INTERNAL"},
                                       {14, "UNAVAILABLE"},
                                       {15, "DATA_LOSS"},
                                       {16, "UNAUTHENTICATED"}};

  /*
    print_array and print_value are used to print out the value of an attribute within a vector.
    These values are held in a variant which makes the process of printing them much more
    complicated.
  */

  template <typename T>
  void print_array(sdktrace::SpanDataAttributeValue &value)
  {
    sout_ << '[';
    auto s    = nostd::get<std::vector<T>>(value);
    size_t i  = 1;
    size_t sz = s.size();
    for (auto v : s)
    {
      sout_ << v;
      if (i != sz)
        sout_ << ',';
      i++;
    };
    sout_ << ']';
  }

  void print_value(sdktrace::SpanDataAttributeValue &value)
  {
    if (nostd::holds_alternative<bool>(value))
    {
      sout_ << nostd::get<bool>(value);
    }
    else if (nostd::holds_alternative<int64_t>(value))
    {
      sout_ << nostd::get<int64_t>(value);
    }
    else if (nostd::holds_alternative<uint64_t>(value))
    {
      sout_ << nostd::get<uint64_t>(value);
    }
    else if (nostd::holds_alternative<double>(value))
    {
      sout_ << nostd::get<double>(value);
    }
    else if (nostd::holds_alternative<std::string>(value))
    {
      sout_ << nostd::get<std::string>(value);
    }
    else if (nostd::holds_alternative<std::vector<bool>>(value))
    {
      print_array<bool>(value);
    }
    else if (nostd::holds_alternative<std::vector<int64_t>>(value))
    {
      print_array<int64_t>(value);
    }
    else if (nostd::holds_alternative<std::vector<uint64_t>>(value))
    {
      print_array<uint64_t>(value);
    }
    else if (nostd::holds_alternative<std::vector<double>>(value))
    {
      print_array<double>(value);
    }
    else if (nostd::holds_alternative<std::vector<std::string>>(value))
    {
      print_array<std::string>(value);
    }
  }

  void printAttributes(std::unordered_map<std::string, sdktrace::SpanDataAttributeValue> map)
  {
    int size = map.size();
    int i    = 1;
    for (auto kv : map)
    {
      sout_ << kv.first << ": ";
      print_value(kv.second);

      if (i != size)
        sout_ << ", ";
      i++;
    }
  }
};
}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
