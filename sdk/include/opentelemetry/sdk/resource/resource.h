// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/entity.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

using ResourceAttributes = opentelemetry::sdk::common::AttributeMap;

class Resource
{
public:
  Resource() noexcept;

  Resource(const ResourceAttributes &attributes) noexcept;

  Resource(const ResourceAttributes &attributes, const std::string &schema_url) noexcept;

  /**
   * Constructs a Resource from attributes, a schema URL, and entities.
   *
   * Invalid entities, later entities of a duplicate type, and entities that
   * share attribute keys with a higher-priority (earlier) entity are dropped.
   * Keys owned by surviving entities are removed from unassociated attributes.
   * If any entity survives, the Resource schema URL is taken from those
   * entities (common URL, or empty if they differ); the constructor schema
   * URL is used only when no entity survives.
   */
  Resource(const ResourceAttributes &attributes,
           const std::string &schema_url,
           const std::vector<Entity> &entities) noexcept;

  Resource(const Resource &)            = default;
  Resource(Resource &&)                 = default;
  Resource &operator=(const Resource &) = default;
  Resource &operator=(Resource &&)      = default;

  ~Resource() = default;

  const ResourceAttributes &GetAttributes() const noexcept;
  const std::string &GetSchemaURL() const noexcept;
  const std::vector<Entity> &GetEntities() const noexcept;
  const ResourceAttributes &GetUnassociatedAttributes() const noexcept;

  /**
   * Returns a new, merged {@link Resource} by merging the current Resource
   * (old) with the other Resource (updating). In case of a collision, the
   * other Resource takes precedence for unassociated attributes.
   *
   * When neither Resource has entities, attributes and schema URLs follow the
   * historical merge rules. If schema urls collide, the resulting schema url
   * is implementation-defined; this implementation picks @p other.
   *
   * When either Resource has entities, merge follows the entity-aware
   * resource data model: type-rank, description overlay, updating unassociated
   * keys evicting entities, then construction-time key uniqueness.
   *
   * @param other the Resource that will be merged with this.
   * @returns the newly merged Resource.
   */

  Resource Merge(const Resource &other) const noexcept;

  /**
   * Returns a newly created Resource with the specified attributes.
   * It adds (merge) SDK attributes and OTEL attributes before returning.
   * @param attributes for this resource
   * @param schema_url The schema URL for this resource.
   * @returns the newly created Resource.
   */

  static Resource Create(const ResourceAttributes &attributes,
                         const std::string &schema_url = std::string{});

  /**
   * Returns a newly created Resource with the specified attributes and
   * entities. SDK attributes and OTEL attributes are merged in as with the
   * two-argument Create.
   */

  static Resource Create(const ResourceAttributes &attributes,
                         const std::string &schema_url,
                         const std::vector<Entity> &entities);

  /**
   * Returns an Empty resource.
   */

  static Resource &GetEmpty();

  /**
   * Returns a Resource that indentifies the SDK in use.
   */

  static Resource &GetDefault();

private:
  void NormalizeEntities(const std::vector<Entity> &entities) noexcept;
  void RefreshFlattenedAttributes() noexcept;

  std::vector<Entity> entities_;
  ResourceAttributes unassociated_attributes_;
  ResourceAttributes attributes_;
  std::string schema_url_;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
