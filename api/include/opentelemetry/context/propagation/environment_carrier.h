// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
namespace propagation
{

/**
 * EnvironmentCarrier is a TextMapCarrier that reads from and writes to environment variables.
 *
 * This carrier enables context propagation across process boundaries using environment variables
 * as specified in the OpenTelemetry specification:
 * https://opentelemetry.io/docs/specs/otel/context/env-carriers/
 *
 * The carrier supports two usage scenarios:
 *
 * 1. Extract (default constructor): Reads context from environment variables
 *    - Used with TextMapPropagator::Extract() to retrieve parent context
 *    - Get() reads from TRACEPARENT, TRACESTATE, BAGGAGE environment variables
 *    - Set() is a no-op (extraction doesn't modify environment)
 *    - Values are cached on first access for lifetime management
 *
 * 2. Inject (map constructor): Writes context to a provided map
 *    - Used with TextMapPropagator::Inject() to prepare context for child processes
 *    - Get() reads from process environment (not the map - Inject doesn't call Get)
 *    - Set() writes to the provided std::map that can be passed to exec/spawn functions
 *
 * Environment variable naming:
 * - Propagators use lowercase header names: traceparent, tracestate, baggage
 * - Environment variables use uppercase names: TRACEPARENT, TRACESTATE, BAGGAGE
 * - The carrier automatically converts between formats
 *
 * Example usage (Extract):
 * @code
 * auto propagator = GlobalTextMapPropagator::GetGlobalPropagator();
 * EnvironmentCarrier carrier;  // Default constructor
 * auto context = propagator->Extract(carrier, Context{});
 * @endcode
 *
 * Example usage (Inject):
 * @code
 * std::map<std::string, std::string> child_env;
 * EnvironmentCarrier carrier(child_env);  // Map constructor
 * auto propagator = GlobalTextMapPropagator::GetGlobalPropagator();
 * propagator->Inject(carrier, RuntimeContext::GetCurrent());
 * // child_env now contains TRACEPARENT, TRACESTATE, BAGGAGE
 * // Pass to exec/spawn function
 * @endcode
 *
 */
class EnvironmentCarrier : public TextMapCarrier
{
public:
  /**
   * Constructs an EnvironmentCarrier for Extract operations.
   * Reads environment variables from the current process environment.
   * Used with TextMapPropagator::Extract() to retrieve parent context.
   * Set() calls will be no-ops.
   */
  EnvironmentCarrier() noexcept : env_map_ptr_(nullptr) {}

  /**
   * Constructs an EnvironmentCarrier for Inject operations.
   * Writes environment variables to the provided map.
   * Used with TextMapPropagator::Inject() to prepare context for child processes.
   *
   * @param env_map Reference to a map that will receive environment variables.
   *                The map is owned by the caller and must outlive this carrier.
   */
  explicit EnvironmentCarrier(std::map<std::string, std::string> &env_map) noexcept
      : env_map_ptr_(&env_map)
  {}

  /**
   * Gets the value associated with the passed key.
   * Always reads from process environment variables (with caching).
   *
   * @param key The key to look up (e.g., "traceparent").
   *            Automatically converted to uppercase (e.g., "TRACEPARENT").
   * @return The value associated with the key, or an empty string if not found.
   */
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    std::string env_name = ToEnvName(key);

    // Check cache first
    auto cache_it = cache_.find(std::string(key));
    if (cache_it != cache_.end())
    {
      return cache_it->second;
    }

    // Read from environment
    const char *value = std::getenv(env_name.c_str());
    if (value != nullptr)
    {
      // Cache for lifetime management (string_view requires stable storage)
      cache_[std::string(key)] = std::string(value);
      return cache_[std::string(key)];
    }
    return "";
  }

  /**
   * Stores the key-value pair.
   * If a map was provided at construction, writes to the map.
   * Otherwise, this operation is a no-op.
   *
   * @param key The key to set (e.g., "traceparent").
   *            Automatically converted to uppercase (e.g., "TRACEPARENT").
   * @param value The value to set.
   */
  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    if (env_map_ptr_ == nullptr)
    {
      return;  // No-op if map not provided (Extract scenario)
    }

    std::string env_name = ToEnvName(key);
    (*env_map_ptr_)[env_name] = std::string(value);
  }

  /**
   * Lists all keys in the carrier.
   *
   * Returns only known OTel environment variables (traceparent, tracestate, baggage)
   * that exist in the environment.
   *
   * @param callback Function to call for each key. Return false to stop iteration.
   * @return true if all keys were processed, false if iteration was stopped early.
   */
  bool Keys(nostd::function_ref<bool(nostd::string_view)> callback) const noexcept override
  {
    // Only check for known OTel environment variables
    // to avoid exposing all process environment variables
    const char *known_keys[] = {"traceparent", "tracestate", "baggage"};
    for (const char *key : known_keys)
    {
      if (!Get(key).empty())
      {
        if (!callback(key))
        {
          return false;
        }
      }
    }
    return true;
  }

private:
  std::map<std::string, std::string> *env_map_ptr_;  // For Inject
  mutable std::map<std::string, std::string> cache_; // For Extract

  /**
   * Converts a header name to an environment variable name.
   * Example: "traceparent" -> "TRACEPARENT"
   *
   * @param key The header name (lowercase).
   * @return The environment variable name (uppercase).
   */
  static std::string ToEnvName(nostd::string_view key)
  {
    std::string env_name(key);
    std::transform(env_name.begin(), env_name.end(), env_name.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return env_name;
  }
};

}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
