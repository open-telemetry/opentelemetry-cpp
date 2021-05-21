/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/version.h"

#include <iostream>
#include <map>
#include <sstream>

namespace nostd     = opentelemetry::nostd;
namespace sdktrace  = opentelemetry::sdk::trace;
namespace sdkcommon = opentelemetry::sdk::common;

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

  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  std::ostream &sout_;
  bool isShutdown_ = false;

  // Mapping status number to the string from api/include/opentelemetry/trace/canonical_code.h
  std::map<int, std::string> statusMap{{0, "Unset"}, {1, "Ok"}, {2, "Error"}};

  /*
    print_value is used to print out the value of an attribute within a vector.
    These values are held in a variant which makes the process of printing them much more
    complicated.
  */

  template <typename T>
  void print_value(const T &item)
  {
    sout_ << item;
  }

  template <typename T>
  void print_value(const std::vector<T> &vec)
  {
    sout_ << '[';
    size_t i  = 1;
    size_t sz = vec.size();
    for (auto v : vec)
    {
      sout_ << v;
      if (i != sz)
        sout_ << ',';
      i++;
    };
    sout_ << ']';
  }

// Prior to C++14, generic lambda is not available so fallback to functor.
#if __cplusplus < 201402L

  class OwnedAttributeValueVisitor
  {
  public:
    OwnedAttributeValueVisitor(OStreamSpanExporter &exporter) : exporter_(exporter) {}

    template <typename T>
    void operator()(T &&arg)
    {
      exporter_.print_value(arg);
    }

  private:
    OStreamSpanExporter &exporter_;
  };

#endif

  void print_value(const sdkcommon::OwnedAttributeValue &value)
  {
#if __cplusplus < 201402L
    nostd::visit(OwnedAttributeValueVisitor(*this), value);
#else
    nostd::visit(
        [this](auto &&arg) {
          /* explicit this is needed by some gcc versions (observed with v5.4.0)*/
          this->print_value(arg);
        },
        value);
#endif
  }

  // various print helpers
  void printAttributes(const std::unordered_map<std::string, sdkcommon::OwnedAttributeValue> &map,
                       const std::string prefix = "\n\t");

  void printEvents(const std::vector<sdktrace::SpanDataEvent> &events);

  void printLinks(const std::vector<sdktrace::SpanDataLink> &links);
};
}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
