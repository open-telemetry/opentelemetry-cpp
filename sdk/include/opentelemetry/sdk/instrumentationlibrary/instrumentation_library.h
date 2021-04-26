// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

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
   * @returns the newly created InstrumentationLibrary.
   */
  static nostd::unique_ptr<InstrumentationLibrary> create(nostd::string_view name,
                                                          nostd::string_view version = "")
  {
    return nostd::unique_ptr<InstrumentationLibrary>(
        new InstrumentationLibrary{std::string{name}, std::string{version}});
  }

  /**
   * Compare 2 instrumentation libraries.
   * @param other the instrumentation library to compare to.
   * @returns true if the 2 instrumentation libraries are equal, false otherwise.
   */
  bool operator==(const InstrumentationLibrary &other) const
  {
    return equal(other.name_, other.version_);
  }

  /**
   * Check whether the instrumentation library has given name and version.
   * This could be used to check version equality and avoid heap allocation.
   * @param name name of the instrumentation library to compare.
   * @param version version of the instrumentatoin library to compare.
   * @returns true if name and version in this instrumentation library are equal with the given name
   * and version.
   */
  bool equal(const nostd::string_view name, const nostd::string_view version) const
  {
    return this->name_ == name && this->version_ == version;
  }

  const std::string &GetName() const { return name_; }
  const std::string &GetVersion() const { return version_; }

private:
  InstrumentationLibrary(nostd::string_view name, nostd::string_view version)
      : name_(name), version_(version)
  {}

private:
  std::string name_;
  std::string version_;
};

}  // namespace instrumentationlibrary
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
