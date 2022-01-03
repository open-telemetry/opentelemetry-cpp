// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/version.h"

#include <iostream>
#include <map>
#include <sstream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

/**
 * The OStreamSpanExporter exports span data through an ostream
 */
class OStreamSpanExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * Create an OStreamSpanExporter. This constructor takes in a reference to an ostream that the
   * export() function will send span data into.
   * The default ostream is set to stdout
   */
  explicit OStreamSpanExporter(std::ostream &sout = std::cout) noexcept;

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &spans) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  std::ostream &sout_;
  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;

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

  void print_value(const opentelemetry::sdk::common::OwnedAttributeValue &value)
  {
#if __cplusplus < 201402L
    opentelemetry::nostd::visit(OwnedAttributeValueVisitor(*this), value);
#else
    opentelemetry::nostd::visit(
        [this](auto &&arg) {
          /* explicit this is needed by some gcc versions (observed with v5.4.0)*/
          this->print_value(arg);
        },
        value);
#endif
  }

  // various print helpers
  void printAttributes(
      const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> &map,
      const std::string prefix = "\n\t");

  void printEvents(const std::vector<opentelemetry::sdk::trace::SpanDataEvent> &events);

  void printLinks(const std::vector<opentelemetry::sdk::trace::SpanDataLink> &links);

  void printResources(const opentelemetry::sdk::resource::Resource &resources);

  void printInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library);
};
}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
