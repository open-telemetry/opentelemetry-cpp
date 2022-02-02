// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace instrumentationlibrary
{

class InstrumentationLibrary
{
public:
  InstrumentationLibrary(const InstrumentationLibrary &) = default;

  /**
   * Returns a newly created InstrumentationLibrary with the specified library name and version.
   * @param name name of the instrumentation library.
   * @param version version of the instrumentation library.
   * @param schema_url schema url of the telemetry emitted by the library.
   * @returns the newly created InstrumentationLibrary.
   */
  static nostd::unique_ptr<InstrumentationLibrary> Create(nostd::string_view name,
                                                          nostd::string_view version    = "",
                                                          nostd::string_view schema_url = "")
  {
    return nostd::unique_ptr<InstrumentationLibrary>(
        new InstrumentationLibrary{name, version, schema_url});
  }

  /**
   * Compare 2 instrumentation libraries.
   * @param other the instrumentation library to compare to.
   * @returns true if the 2 instrumentation libraries are equal, false otherwise.
   */
  bool operator==(const InstrumentationLibrary &other) const
  {
    return equal(other.name_, other.version_, other.schema_url_);
  }

  /**
   * Check whether the instrumentation library has given name and version.
   * This could be used to check version equality and avoid heap allocation.
   * @param name name of the instrumentation library to compare.
   * @param version version of the instrumentatoin library to compare.
   * @param schema_url schema url of the telemetry emitted by the library.
   * @returns true if name and version in this instrumentation library are equal with the given name
   * and version.
   */
  bool equal(const nostd::string_view name,
             const nostd::string_view version,
             const nostd::string_view schema_url = "") const
  {
    return this->name_ == name && this->version_ == version && this->schema_url_ == schema_url;
  }

  const std::string &GetName() const { return name_; }
  const std::string &GetVersion() const { return version_; }
  const std::string &GetSchemaURL() const { return schema_url_; }

private:
  InstrumentationLibrary(nostd::string_view name,
                         nostd::string_view version,
                         nostd::string_view schema_url = "")
      : name_(name), version_(version), schema_url_(schema_url)
  {}

private:
  std::string name_;
  std::string version_;
  std::string schema_url_;
};

}  // namespace instrumentationlibrary
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
