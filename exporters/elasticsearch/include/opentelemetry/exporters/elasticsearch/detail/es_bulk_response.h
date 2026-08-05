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
 * Whether an acknowledged operation status is one that applied the operation. Elasticsearch
 * answers an index operation with 200 or 201, so 2xx is the whole band.
 *
 * Compared in the type the number was parsed as. Narrowing to int first is not safe here: the
 * value comes from the server, is_number_integer() is true for unsigned as well, and 2^32 + 200
 * narrows back into the band on a 32 bit int.
 */
inline bool IsAcknowledgedStatus(const nlohmann::json &status) noexcept
{
  if (status.is_number_unsigned())
  {
    const auto value = status.get<nlohmann::json::number_unsigned_t>();
    return value >= 200U && value <= 299U;
  }

  const auto value = status.get<nlohmann::json::number_integer_t>();
  return value >= 200 && value <= 299;
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
 *         expected_items index results that each acknowledge a 2xx status and carry no error
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
    if (status_code < 200 || status_code > 299)
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

      const auto error = operation->find("error");
      if (item_error == nullptr && error != operation->end())
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
