// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
namespace detail
{

/**
 * The band a status has to fall in to say the thing it answers was applied. Elasticsearch
 * answers the bulk request with 200 and an index operation with 200 or 201, so 2xx is the whole
 * of it on both, and this says so once for both rather than twice with two spellings.
 *
 * Two overloads rather than one signed parameter, because an operation status arrives through
 * nlohmann::json and is compared in the type it was parsed as: is_number_integer() is true for
 * unsigned as well, and 2^32 + 200 narrows back into the band on a 32 bit int.
 */
inline bool IsSuccessStatus(nlohmann::json::number_unsigned_t value) noexcept
{
  return value >= 200U && value <= 299U;
}

inline bool IsSuccessStatus(nlohmann::json::number_integer_t value) noexcept
{
  return value >= 200 && value <= 299;
}

/** Whether an acknowledged operation status is one that applied the operation. */
inline bool IsAcknowledgedStatus(const nlohmann::json &status) noexcept
{
  if (status.is_number_unsigned())
  {
    return IsSuccessStatus(status.get<nlohmann::json::number_unsigned_t>());
  }

  return IsSuccessStatus(status.get<nlohmann::json::number_integer_t>());
}

/**
 * Decide whether an Elasticsearch bulk response reports the whole batch as written.
 *
 * Callers include noexcept response handlers, so nothing may escape. Anything that stops the body
 * being inspected counts as a failed export.
 *
 * @param status_code the response status, which the caller passes rather than this reading the
 *        body alone: "errors" describes item outcomes and cannot override a transport error
 * @param body the raw response body
 * @param expected_items the number of index operations the request submitted
 * @param failure_reason a best-effort explanation when this returns false
 * @return true only when the status is 2xx, "errors" is false, and "items" holds exactly
 *         expected_items index results that each name a target index, acknowledge a 2xx status
 *         and carry no error
 */
inline bool IsBulkResponseSuccessful(int status_code,
                                     const std::string &body,
                                     std::size_t expected_items,
                                     std::string &failure_reason) noexcept
{
  failure_reason.clear();
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    // Inside the try: building a reason allocates.
    // The same band as an operation status, through the same predicate. The cast picks the
    // signed overload, which an int reaches without losing anything.
    if (!IsSuccessStatus(static_cast<nlohmann::json::number_integer_t>(status_code)))
    {
      failure_reason = "unexpected HTTP status " + std::to_string(status_code);
      return false;
    }

    const nlohmann::json parsed = nlohmann::json::parse(body, nullptr, false);
    if (parsed.is_discarded() || !parsed.is_object())
    {
      failure_reason = "the response body is not a JSON object";
      return false;
    }

    const auto errors = parsed.find("errors");
    if (errors == parsed.end() || !errors->is_boolean())
    {
      failure_reason = "the response body has no boolean \"errors\" field";
      return false;
    }

    // The request is an unfiltered /_bulk, so "items" is always there and holds one entry per
    // operation. The next three checks decide whether this body answers the request that was sent.
    const auto items = parsed.find("items");
    if (items == parsed.end() || !items->is_array())
    {
      failure_reason = "the response body has no \"items\" array";
      return false;
    }

    if (items->size() != expected_items)
    {
      failure_reason = "the response acknowledges " + std::to_string(items->size()) + " of " +
                       std::to_string(expected_items) + " submitted operations";
      return false;
    }

    // Each entry is keyed by the action it answers, and every record goes out as an index
    // operation. Read before "errors", which a body that is not this answer does not get to decide.
    // An operation that was not applied says so twice, through its status and through an "error"
    // member. Both are read here so that neither reading depends on the "errors" flag, which a
    // response that contradicts itself also controls.
    const nlohmann::json *rejected   = nullptr;
    const nlohmann::json *item_error = nullptr;
    for (const auto &item : *items)
    {
      if (!item.is_object() || item.size() != 1)
      {
        failure_reason = "the response has an \"items\" entry that is not one operation result";
        return false;
      }

      const auto operation = item.find("index");
      if (operation == item.end() || !operation->is_object())
      {
        failure_reason = "the response does not acknowledge the submitted index operation";
        return false;
      }

      // Elasticsearch names the index it wrote to in every index result, whether the operation
      // applied or not. Presence and type only: the name it reports is the index the write
      // resolved to, which an alias or a date math index makes different from the one that was
      // submitted, so it is not something to compare against.
      const auto target = operation->find("_index");
      if (target == operation->end() || !target->is_string())
      {
        failure_reason = "the response acknowledges an index operation with no target index";
        return false;
      }

      const auto status = operation->find("status");
      if (status == operation->end() || !status->is_number_integer())
      {
        failure_reason = "the response acknowledges an index operation with no status";
        return false;
      }

      if (rejected == nullptr && !IsAcknowledgedStatus(*status))
      {
        rejected = &(*status);
      }

      // A null holds no cause, and a serialiser that writes absent optionals as null is saying
      // the operation applied, which is what the flag says too. Only a cause contradicts it.
      const auto error = operation->find("error");
      if (item_error == nullptr && error != operation->end() && !error->is_null())
      {
        item_error = &(*error);
      }
    }

    if (!errors->get<bool>())
    {
      // A false flag claims every operation was applied; hold the items to that claim.
      if (rejected != nullptr)
      {
        failure_reason =
            "the response reports no errors but acknowledges operation status " + rejected->dump();
        return false;
      }
      if (item_error != nullptr)
      {
        failure_reason =
            "the response reports no errors but an item carries error " + item_error->dump();
        return false;
      }
      return true;
    }

    // Name the first item error rather than only saying that something failed.
    if (item_error != nullptr)
    {
      failure_reason = "at least one item failed, first error: " + item_error->dump();
      return false;
    }

    failure_reason = "the response reports errors";
    return false;
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  catch (...)
  {
    return false;
  }
#endif
}

}  // namespace detail
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
