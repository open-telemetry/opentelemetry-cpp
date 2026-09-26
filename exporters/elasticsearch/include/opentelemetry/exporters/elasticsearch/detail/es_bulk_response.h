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
 * What the bulk request itself has to answer with. Elasticsearch answers a bulk request with 200,
 * and the rest of the 2xx band does not say the batch was written. 202 is the one that matters:
 * it says the request was accepted and the processing is not finished, and may never be. Reading
 * that as success is not a conservative reading of a vague answer, it is the opposite of what the
 * answer says, and a success here lets the caller drop the records it just handed over.
 */
inline bool IsSuccessfulBulkStatus(int status_code) noexcept
{
  return 200 == status_code;
}

/**
 * Whether an acknowledged operation status is one that applied the operation. Elasticsearch
 * answers 201 when it created the document and 200 when it replaced an existing one.
 *
 * Comparing the json value rather than extracting one keeps this noexcept without relying on the
 * type check to make an extraction safe. The check still has to be here: without it a float 200.5
 * would answer for 200.
 */
inline bool IsAcknowledgedStatus(const nlohmann::json &status) noexcept
{
  return status.is_number_integer() && (status == 200 || status == 201);
}

/**
 * Decide whether an Elasticsearch bulk response reports the whole batch as written.
 *
 * Callers include noexcept response handlers, so nothing may escape. Anything that stops the body
 * being inspected counts as a failed export.
 *
 * This reads the fields the bulk response documents to decide an outcome. It is not a check
 * against a responder that is trying to be believed: a repeated key, for one, is folded before
 * this sees the document, so a body carrying both "errors": true and "errors": false arrives as
 * whichever one the parser kept, and nothing here can tell that the other was ever sent.
 *
 * @param status_code the response status, which the caller passes rather than this reading the
 *        body alone: "errors" describes item outcomes and cannot override a transport error
 * @param body the raw response body
 * @param expected_items the number of index operations the request submitted
 * @param failure_reason a best-effort explanation when this returns false
 * @return true only when the status is 200, "errors" is false, and "items" holds exactly
 *         expected_items index results that each name a target index, acknowledge a status the
 *         operation applied under, and carry no error
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
    // Not the band an operation status is held to. This one answers for the request, and an
    // operation that applied answers 201 for a document it created, which the request itself
    // never says.
    if (!IsSuccessfulBulkStatus(status_code))
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
