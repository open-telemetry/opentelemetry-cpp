// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/semconv/incubating/process_attributes.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/semconv/telemetry_attributes.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
namespace
{

bool EntityOwnsKey(const Entity &entity, const std::string &key)
{
  return entity.GetIdentity().find(key) != entity.GetIdentity().end() ||
         entity.GetDescription().find(key) != entity.GetDescription().end();
}

bool CanMergeEntities(const Entity &existing, const Entity &incoming)
{
  return existing.GetIdentity() == incoming.GetIdentity() &&
         existing.GetSchemaURL() == incoming.GetSchemaURL();
}

Entity OverlayDescription(const Entity &existing, const Entity &incoming)
{
  ResourceAttributes description = existing.GetDescription();
  for (const auto &kv : incoming.GetDescription())
  {
    description[kv.first] = kv.second;
  }
  return Entity(existing.GetType(), existing.GetIdentity(), description, existing.GetSchemaURL());
}

std::unordered_map<std::string, std::size_t> BuildTypeRanks(const Resource &old_resource,
                                                            const Resource &updating)
{
  std::unordered_map<std::string, std::size_t> rank;
  const auto &updating_entities = updating.GetEntities();
  for (std::size_t i = 0; i < updating_entities.size(); ++i)
  {
    const std::string &type = updating_entities[i].GetType();
    if (rank.find(type) == rank.end())
    {
      rank[type] = i;
    }
  }
  std::size_t old_only = updating_entities.size();
  for (const auto &entity : old_resource.GetEntities())
  {
    if (rank.find(entity.GetType()) == rank.end())
    {
      rank[entity.GetType()] = old_only++;
    }
  }
  return rank;
}

}  // namespace

Resource::Resource() noexcept : entities_(), unassociated_attributes_(), schema_url_()
{
  RefreshFlattenedAttributes();
}

Resource::Resource(const ResourceAttributes &attributes) noexcept
    : entities_(), unassociated_attributes_(attributes), schema_url_()
{
  RefreshFlattenedAttributes();
}

Resource::Resource(const ResourceAttributes &attributes, const std::string &schema_url) noexcept
    : entities_(), unassociated_attributes_(attributes), schema_url_(schema_url)
{
  RefreshFlattenedAttributes();
}

Resource::Resource(const ResourceAttributes &attributes,
                   const std::string &schema_url,
                   const std::vector<Entity> &entities) noexcept
    : entities_(), unassociated_attributes_(attributes), schema_url_(schema_url)
{
  NormalizeEntities(entities);
  RefreshFlattenedAttributes();
}

void Resource::NormalizeEntities(const std::vector<Entity> &entities) noexcept
{
  std::vector<Entity> accepted;
  std::unordered_set<std::string> accepted_types;
  std::unordered_set<std::string> accepted_keys;
  accepted.reserve(entities.size());

  for (const auto &entity : entities)
  {
    if (!entity.IsValid())
    {
      OTEL_INTERNAL_LOG_WARN("[Resource] Dropping invalid Entity.");
      continue;
    }

    if (accepted_types.find(entity.GetType()) != accepted_types.end())
    {
      OTEL_INTERNAL_LOG_WARN("[Resource] Dropping Entity of duplicate type.");
      continue;
    }

    bool key_conflict = false;
    for (const auto &kv : entity.GetIdentity())
    {
      if (accepted_keys.find(kv.first) != accepted_keys.end())
      {
        key_conflict = true;
        break;
      }
    }
    if (!key_conflict)
    {
      for (const auto &kv : entity.GetDescription())
      {
        if (accepted_keys.find(kv.first) != accepted_keys.end())
        {
          key_conflict = true;
          break;
        }
      }
    }
    if (key_conflict)
    {
      OTEL_INTERNAL_LOG_WARN("[Resource] Dropping Entity due to attribute key conflict.");
      continue;
    }

    accepted_types.insert(entity.GetType());
    for (const auto &kv : entity.GetIdentity())
    {
      accepted_keys.insert(kv.first);
    }
    for (const auto &kv : entity.GetDescription())
    {
      accepted_keys.insert(kv.first);
    }
    accepted.push_back(entity);
  }

  entities_ = std::move(accepted);

  for (const auto &key : accepted_keys)
  {
    unassociated_attributes_.erase(key);
  }

  if (!entities_.empty())
  {
    const std::string &common_schema_url = entities_.front().GetSchemaURL();
    bool all_equal                       = true;
    for (const auto &entity : entities_)
    {
      if (entity.GetSchemaURL() != common_schema_url)
      {
        all_equal = false;
        break;
      }
    }
    schema_url_ = all_equal ? common_schema_url : std::string{};
  }
}

void Resource::RefreshFlattenedAttributes() noexcept
{
  attributes_.clear();
  for (const auto &entity : entities_)
  {
    attributes_.insert(entity.GetIdentity().begin(), entity.GetIdentity().end());
    attributes_.insert(entity.GetDescription().begin(), entity.GetDescription().end());
  }
  attributes_.insert(unassociated_attributes_.begin(), unassociated_attributes_.end());
}

Resource Resource::Merge(const Resource &other) const noexcept
{
  if (entities_.empty() && other.entities_.empty())
  {
    ResourceAttributes merged_resource_attributes(other.attributes_);
    merged_resource_attributes.insert(attributes_.begin(), attributes_.end());
    return Resource(merged_resource_attributes,
                    other.schema_url_.empty() ? schema_url_ : other.schema_url_);
  }

  const Resource &updating = other;
  auto rank                = BuildTypeRanks(*this, updating);

  std::vector<Entity> merged_entities = GetEntities();
  for (const auto &incoming : updating.GetEntities())
  {
    if (!incoming.IsValid())
    {
      continue;
    }

    auto existing = std::find_if(
        merged_entities.begin(), merged_entities.end(),
        [&incoming](const Entity &entity) { return entity.GetType() == incoming.GetType(); });
    if (existing != merged_entities.end())
    {
      if (CanMergeEntities(*existing, incoming))
      {
        *existing = OverlayDescription(*existing, incoming);
      }
      else
      {
        OTEL_INTERNAL_LOG_WARN(
            "[Resource] Dropping Entity that cannot merge with an existing type.");
      }
    }
    else
    {
      merged_entities.push_back(incoming);
    }
  }

  ResourceAttributes unassociated(updating.GetUnassociatedAttributes());
  unassociated.insert(GetUnassociatedAttributes().begin(), GetUnassociatedAttributes().end());

  std::vector<Entity> after_eviction;
  after_eviction.reserve(merged_entities.size());
  for (const auto &entity : merged_entities)
  {
    bool evicted = false;
    for (const auto &kv : updating.GetUnassociatedAttributes())
    {
      if (EntityOwnsKey(entity, kv.first))
      {
        evicted = true;
        break;
      }
    }
    if (evicted)
    {
      OTEL_INTERNAL_LOG_WARN("[Resource] Dropping Entity due to updating unassociated attribute.");
      continue;
    }
    after_eviction.push_back(entity);
  }

  std::stable_sort(
      after_eviction.begin(), after_eviction.end(), [&rank](const Entity &lhs, const Entity &rhs) {
        auto left              = rank.find(lhs.GetType());
        auto right             = rank.find(rhs.GetType());
        std::size_t left_rank  = left == rank.end() ? static_cast<std::size_t>(-1) : left->second;
        std::size_t right_rank = right == rank.end() ? static_cast<std::size_t>(-1) : right->second;
        return left_rank < right_rank;
      });

  const std::string classic_schema =
      updating.GetSchemaURL().empty() ? GetSchemaURL() : updating.GetSchemaURL();
  return Resource(unassociated, classic_schema, after_eviction);
}

Resource Resource::Create(const ResourceAttributes &attributes, const std::string &schema_url)
{
  return Create(attributes, schema_url, {});
}

Resource Resource::Create(const ResourceAttributes &attributes,
                          const std::string &schema_url,
                          const std::vector<Entity> &entities)
{
  static auto otel_resource = OTELResourceDetector().Detect();
  auto resource =
      Resource::GetDefault().Merge(otel_resource).Merge(Resource{attributes, schema_url, entities});

  if (resource.attributes_.find(semconv::service::kServiceName) == resource.attributes_.end())
  {
    std::string default_service_name = "unknown_service";
    auto it_process_executable_name =
        resource.attributes_.find(semconv::process::kProcessExecutableName);
    if (it_process_executable_name != resource.attributes_.end())
    {
      default_service_name += ":" + nostd::get<std::string>(it_process_executable_name->second);
    }
    resource.unassociated_attributes_[semconv::service::kServiceName] = default_service_name;
    resource.RefreshFlattenedAttributes();
  }
  return resource;
}

Resource &Resource::GetEmpty()
{
  static Resource empty_resource;
  return empty_resource;
}

Resource &Resource::GetDefault()
{
  static Resource default_resource(
      {{semconv::telemetry::kTelemetrySdkLanguage, "cpp"},
       {semconv::telemetry::kTelemetrySdkName, "opentelemetry"},
       {semconv::telemetry::kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION}},
      std::string{});
  return default_resource;
}

const ResourceAttributes &Resource::GetAttributes() const noexcept
{
  return attributes_;
}

const std::string &Resource::GetSchemaURL() const noexcept
{
  return schema_url_;
}

const std::vector<Entity> &Resource::GetEntities() const noexcept
{
  return entities_;
}

const ResourceAttributes &Resource::GetUnassociatedAttributes() const noexcept
{
  return unassociated_attributes_;
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
