// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

#include <functional>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace instrumentationscope
{

class InstrumentationScope
{
public:
  InstrumentationScope(const InstrumentationScope &) = default;

  /**
   * Returns a newly created InstrumentationScope with the specified library name and version.
   * @param name name of the instrumentation scope.
   * @param version version of the instrumentation scope.
   * @param schema_url schema url of the telemetry emitted by the library.
   * @returns the newly created InstrumentationScope.
   */
  static nostd::unique_ptr<InstrumentationScope> Create(nostd::string_view name,
                                                        nostd::string_view version    = "",
                                                        nostd::string_view schema_url = "")
  {
    return nostd::unique_ptr<InstrumentationScope>(
        new InstrumentationScope{name, version, schema_url});
  }

  std::size_t HashCode() const noexcept { return hash_code_; }

  /**
   * Compare 2 instrumentation libraries.
   * @param other the instrumentation scope to compare to.
   * @returns true if the 2 instrumentation libraries are equal, false otherwise.
   */
  bool operator==(const InstrumentationScope &other) const
  {
    return equal(other.name_, other.version_, other.schema_url_);
  }

  /**
   * Check whether the instrumentation scope has given name and version.
   * This could be used to check version equality and avoid heap allocation.
   * @param name name of the instrumentation scope to compare.
   * @param version version of the instrumentation scope to compare.
   * @param schema_url schema url of the telemetry emitted by the scope.
   * @returns true if name and version in this instrumentation scope are equal with the given name
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
  InstrumentationScope(nostd::string_view name,
                       nostd::string_view version,
                       nostd::string_view schema_url = "")
      : name_(name), version_(version), schema_url_(schema_url)
  {
    std::string hash_data;
    hash_data.reserve(name_.size() + version_.size() + schema_url_.size());
    hash_data += name_;
    hash_data += version_;
    hash_data += schema_url_;
    hash_code_ = std::hash<std::string>{}(hash_data);
  }

private:
  std::string name_;
  std::string version_;
  std::string schema_url_;
  std::size_t hash_code_;
};

}  // namespace instrumentationscope
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
