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

#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/version.h"

#include <memory>
#include <sstream>
#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

using ResourceAttributes =
    std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>;

class Resource
{
public:
  Resource(const Resource &) = default;

  const ResourceAttributes &GetAttributes() const noexcept;

  /**
   * Returns a new, merged {@link Resource} by merging the current Resource
   * with the other Resource. In case of a collision, current Resource takes
   * precedence.
   *
   * @param other the Resource that will be merged with this.
   * @returns the newly merged Resource.
   */

  Resource Merge(const Resource &other) noexcept;

  /**
   * Returns a newly created Resource with the specified attributes.
   * It adds (merge) SDK attributes and OTEL attributes before returning.
   * @param attributes for this resource
   * @returns the newly created Resource.
   */

  static Resource Create(const ResourceAttributes &attributes);

  /**
   * Returns an Empty resource.
   */

  static Resource &GetEmpty();

  /**
   * Returns a Resource that indentifies the SDK in use.
   */

  static Resource &GetDefault();

protected:
  /**
   * The constructor is protected and only for use internally by the class and
   * inside ResourceDetector class.
   * Users should use the Create factory method to obtain a Resource
   * instance.
   */
  Resource(const ResourceAttributes &attributes = ResourceAttributes()) noexcept;

private:
  ResourceAttributes attributes_;

  friend class OTELResourceDetector;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE