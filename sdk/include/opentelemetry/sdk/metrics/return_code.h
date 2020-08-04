#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

/**
 * ReturnCodes Class
 *
 * This class provides an expansion on the possible return codes
 * for the Export() function in the PrometheusExporter class.
 * Previously, only SUCCESS and FAILURE were available as
 * return codes; FAILURE has been expanded to specify what type
 * of failure has occurred.
 */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
enum class ReturnCode : uint8_t
{
  /**
   * Batch exported successfully.
   */
  SUCCESS = 0,

  /**
   * The collection does not have enough space to receive the export batch.
   */
  FAILURE_FULL_COLLECTION = 1,

  /**
   * The export has timed out.
   */
  FAILURE_TIMEOUT = 2,

  /**
   * The export() function was passed an invalid argument.
   */
  FAILURE_INVALID_ARGUMENT = 3,

  /**
   * The exporter is already shutdown.
   */
  FAILURE_ALREADY_SHUTDOWN = 4,
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE